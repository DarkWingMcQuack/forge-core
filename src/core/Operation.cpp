#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <daemon/DaemonBase.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>
#include <vector>

using buddy::core::Entry;
using utilxx::Result;
using utilxx::Opt;
using buddy::core::Transaction;
using buddy::daemon::DaemonBase;
using buddy::daemon::DaemonError;
using buddy::core::parseEntry;

auto buddy::core::getEntryKey(const Operation& operation)
    -> const EntryKey&
{
    return std::visit(
        [](const auto& op)
            -> const EntryKey& {
            return op.getEntryKey();
        },
        operation);
}

auto buddy::core::getEntryKey(Operation&& operation)
    -> EntryKey
{
    return std::visit(
        [](auto&& op) {
            return std::move(op.getEntryKey());
        },
        operation);
}

auto buddy::core::getOwner(const Operation& operation)
    -> const std::string&
{
    return std::visit(
        [](const auto& op)
            -> const std::string& {
            return op.getOwner();
        },
        operation);
}

auto buddy::core::getOwner(Operation&& operation)
    -> std::string
{
    return std::visit(
        [](auto&& op) {
            return std::move(op.getOwner());
        },
        operation);
}

auto buddy::core::getValue(const Operation& operation)
    -> const std::size_t
{
    return std::visit(
        [](auto&& op) {
            return op.getValue();
        },
        operation);
}


auto buddy::core::parseMetadata(const std::vector<std::byte>& metadata,
                                std::size_t block,
                                std::string&& owner,
                                std::size_t value,
                                utilxx::Opt<std::string>&& new_owner_opt)
    -> Opt<Operation>
{
    if(metadata.size() < 10) {
        return std::nullopt;
    }

    return parseEntry(metadata)
        .flatMap([&](auto&& entry)
                     -> utilxx::Opt<Operation> {
            switch(static_cast<std::byte>(metadata[3])) {

            case ENTRY_CREATION_FLAG:
                return Operation{
                    EntryCreationOp{std::move(entry),
                                    std::move(owner),
                                    block,
                                    value}};

            case ENTRY_RENEWAL_FLAG:
                return Operation{
                    EntryRenewalOp{std::move(entry),
                                   std::move(owner),
                                   block,
                                   value}};

            case OWNERSHIP_TRANSFER_FLAG:
                return new_owner_opt
                    .map([&](auto&& new_owner) {
                        return Operation{
                            OwnershipTransferOp{std::move(entry),
                                                std::move(owner),
                                                std::move(new_owner),
                                                block,
                                                value}};
                    });

            case ENTRY_UPDATE_FLAG:
                return Operation{
                    EntryUpdateOp{std::move(entry),
                                  std::move(owner),
                                  block,
                                  value}};

            case ENTRY_DELETION_FLAG:
                return Operation{
                    EntryDeletionOp{std::move(entry),
                                    std::move(owner),
                                    block,
                                    value}};

            default:
                return std::nullopt;
            }
        });
}

auto buddy::core::parseTransactionToEntry(Transaction&& tx,
                                          std::size_t block,
                                          const std::unique_ptr<DaemonBase>& daemon)
    -> Result<Opt<Operation>, DaemonError>
{
    using ResultType = Result<Opt<Operation>, DaemonError>;

    //check if the transaction has exactly one op return
    //output and exactly one input
    //if this is not the case the tx does not repressent a buddy op
    if(!tx.hasExactlyOneOpReturnOutput()
       || !tx.hasExactlyOneInput()) {
        return ResultType{std::nullopt};
    }


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

    //check if the metadata starts with a buddy id
    //if not the tx is not a valid buddy tx and can be ignored
    if(!metadataStartsWithBuddyId(metadata)) {
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
                parseMetadata(std::move(metadata),
                              block,
                              std::move(owner),
                              value,
                              std::move(new_owner_opt))};
        });
}
