#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <utilxx/Result.hpp>
#include <vector>

using forge::core::UMEntry;
using utilxx::Result;
using utilxx::overload;
using utilxx::Opt;
using forge::core::Transaction;
using forge::daemon::ReadOnlyDaemonBase;
using forge::daemon::DaemonError;
using forge::core::parseUMEntry;
using forge::core::FORGE_IDENTIFIER_MASK;

auto forge::core::getEntryKey(const UMEntryOperation& operation)
    -> const EntryKey&
{
    return std::visit(
        [](const auto& op)
            -> const EntryKey& {
            return op.getEntryKey();
        },
        operation);
}

auto forge::core::getEntryKey(UMEntryOperation&& operation)
    -> EntryKey
{
    return std::visit(
        [](auto&& op) {
            return std::move(op.getEntryKey());
        },
        operation);
}

auto forge::core::getUMEntry(const UMEntryOperation& operation)
    -> const UMEntry&
{
    return std::visit(
        [](const auto& op)
            -> const UMEntry& {
            return op.getUMEntry();
        },
        operation);
}

auto forge::core::getUMEntry(UMEntryOperation&& operation)
    -> UMEntry
{
    return std::visit(
        [](auto&& op) {
            return std::move(op.getUMEntry());
        },
        operation);
}

auto forge::core::getOwner(const UMEntryOperation& operation)
    -> const std::string&
{
    return std::visit(
        [](const auto& op)
            -> const std::string& {
            return op.getOwner();
        },
        operation);
}

auto forge::core::getOwner(UMEntryOperation&& operation)
    -> std::string
{
    return std::visit(
        [](auto&& op) {
            return std::move(op.getOwner());
        },
        operation);
}

auto forge::core::getValue(const UMEntryOperation& operation)
    -> const std::int64_t
{
    return std::visit(
        [](auto&& op) {
            return op.getValue();
        },
        operation);
}

auto forge::core::extractOperationFlag(const UMEntryOperation& operation)
    -> std::byte
{
    constexpr static auto flag_extractor =
        overload{
            [](const UMEntryCreationOp&) { return UMENTRY_CREATION_FLAG; },
            [](const UMEntryRenewalOp&) { return UMENTRY_RENEWAL_FLAG; },
            [](const UMEntryOwnershipTransferOp&) { return UMENTRY_OWNERSHIP_TRANSFER_FLAG; },
            [](const UMEntryUpdateOp&) { return UMENTRY_UPDATE_FLAG; },
            [](const UMEntryDeletionOp&) { return UMENTRY_DELETION_FLAG; }};

    return std::visit(flag_extractor,
                      operation);
}


auto forge::core::parseMetadataToUMEntryOp(const std::vector<std::byte>& metadata,
                                std::int64_t block,
                                std::string&& owner,
                                std::int64_t value,
                                utilxx::Opt<std::string>&& new_owner_opt)
    -> Opt<UMEntryOperation>
{
    if(metadata.size() < 10) {
        return std::nullopt;
    }

    return parseUMEntry(metadata)
        .flatMap([&](auto&& entry)
                     -> utilxx::Opt<UMEntryOperation> {
            switch(static_cast<std::byte>(metadata[OPERATION_FLAG_INDEX])) {

            case UMENTRY_CREATION_FLAG:
                return UMEntryOperation{
                    UMEntryCreationOp{std::move(entry),
                                      std::move(owner),
                                      block,
                                      value}};

            case UMENTRY_RENEWAL_FLAG:
                return UMEntryOperation{
                    UMEntryRenewalOp{std::move(entry),
                                     std::move(owner),
                                     block,
                                     value}};

            case UMENTRY_OWNERSHIP_TRANSFER_FLAG:
                return new_owner_opt
                    .map([&](auto&& new_owner) {
                        return UMEntryOperation{
                            UMEntryOwnershipTransferOp{std::move(entry),
                                                       std::move(owner),
                                                       std::move(new_owner),
                                                       block,
                                                       value}};
                    });

            case UMENTRY_UPDATE_FLAG:
                return UMEntryOperation{
                    UMEntryUpdateOp{std::move(entry),
                                    std::move(owner),
                                    block,
                                    value}};

            case UMENTRY_DELETION_FLAG:
                return UMEntryOperation{
                    UMEntryDeletionOp{std::move(entry),
                                      std::move(owner),
                                      block,
                                      value}};

            default:
                return std::nullopt;
            }
        });
}

auto forge::core::parseTransactionToUMEntry(Transaction&& tx,
                                            std::int64_t block,
                                            const daemon::ReadOnlyDaemonBase* daemon)
    -> Result<Opt<UMEntryOperation>, DaemonError>
{
    using ResultType = Result<Opt<UMEntryOperation>, DaemonError>;

    LOG_IF(FATAL, !daemon) << "ReadOnlyDaemonBase pointer is null";

    //check if the transaction has exactly one op return
    //output and exactly one input
    //if this is not the case the tx does not repressent a forge op
    if(!tx.hasExactlyOneOpReturnOutput()
       || !tx.hasExactlyOneInput()) {
        return ResultType{std::nullopt};
    }

    LOG(INFO) << tx.getTxid() << " contains a OP_RETURN output";

    //save, because we checked that the tx has exactly one
    //op return output
    const auto& op_return_output =
        tx.getFirstOpReturnOutput().getValue().get();

    //get optional new owner
    //for ownership transfer
    auto new_owner_opt =
        tx.getFirstNonOpReturnOutput()
            .flatMap([](auto&& ref)
                         -> Opt<std::string> {
                //we only care about outputs with exactly one
                //address
                if(ref.get().getAddresses().size() != 1) {
                    return std::nullopt;
                }
                return ref.get().getAddresses()[0];
            });

    //save, because we have checked that the tx has exactly
    //one input
    auto vin = std::move(tx.getInputs()[0]);

    //value of the op return output
    auto value = op_return_output.getValue();

    //extract the metadata from the output script
    auto output_script = std::move(op_return_output.getHex());
    auto metadata_opt = extractMetadata(std::move(output_script));

    if(!metadata_opt) {
        return ResultType{std::nullopt};
    }

    //get metadata from the op return output
    auto metadata = std::move(metadata_opt.getValue());

    //check if the metadata starts with a forge id
    //if not the tx is not a valid forge tx and can be ignored
    if(!metadataStartsWithForgeId(metadata)) {
        return ResultType{std::nullopt};
    }

    LOG(DEBUG) << "resoving vin from " << vin.getTxid();
    return daemon
        ->resolveTxIn(std::move(vin))
        .flatMap([&](auto&& resolvedVin) {
            //we can only have one input address
            if(resolvedVin.getAddresses().size() != 1) {
                return ResultType{std::nullopt};
            }

            //get owner
            auto owner = std::move(resolvedVin.getAddresses()[0]);

            //parse the metadata nd put it into
            //the ResultType
            return ResultType{
                parseMetadataToUMEntryOp(std::move(metadata),
                              block,
                              std::move(owner),
                              value,
                              std::move(new_owner_opt))};
        });
}


auto forge::core::toMetadata(const UMEntryOperation& op)
    -> std::vector<std::byte>
{
    const auto& entry = getUMEntry(op);
    auto flag = extractOperationFlag(op);

    auto data = forge::core::umEntryToRawData(entry);

    data.insert(std::begin(data),
                flag);

    data.insert(std::begin(data),
                UMENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
