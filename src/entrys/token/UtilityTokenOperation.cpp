#include "g3log/g3log.hpp"
#include "utilxx/Overload.hpp"
#include <array>
#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/token/UtilityTokenDeletionOp.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <entrys/token/UtilityTokenOwnershipTransferOp.hpp>
#include <fmt/core.h>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

using forge::core::UtilityToken;
using forge::core::OPERATION_FLAG_INDEX;
using forge::core::UTILITY_TOKEN_CREATION_FLAG;
using forge::core::UTILITY_TOKEN_DELETION_FLAG;
using forge::core::UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG;
using forge::core::Transaction;
using forge::core::UtilityTokenOperation;
using forge::daemon::ReadOnlyDaemonBase;
using forge::daemon::DaemonError;

auto forge::core::getUtilitToken(const UtilityTokenOperation& op)
    -> const UtilityToken&
{
    return std::visit(
        [](const auto& op) -> decltype(auto) {
            return op.getUtilityToken();
        },
        op);
}

auto forge::core::getUtilityToken(UtilityTokenOperation&& op)
    -> UtilityToken
{
    return std::visit(
        [](auto op) {
            return op.getUtilityToken();
        },
        std::move(op));
}

auto forge::core::getAmount(const UtilityTokenOperation& op)
    -> std::uint64_t
{
    return std::visit(
        [](const auto& op) {
            return op.getAmount();
        },
        op);
}

auto forge::core::getCreator(const UtilityTokenOperation& op)
    -> const std::string&
{
    return std::visit(
        utilxx::overload{
            [](const UtilityTokenOwnershipTransferOp& owner)
                -> decltype(auto) {
                return owner.getCreator();
            },
            [](const auto& owner)
                -> decltype(auto) {
                return owner.getCreator();
            }},
        op);
}

auto forge::core::getCreator(UtilityTokenOperation&& op)
    -> std::string

{
    return std::visit(
        utilxx::overload{
            [](UtilityTokenOwnershipTransferOp&& owner) {
                return owner.getCreator();
            },
            [](auto owner) {
                return owner.getCreator();
            }},
        std::move(op));
}

auto forge::core::extractOperationFlag(const UtilityTokenOperation& op)
    -> std::byte
{
    return std::visit(
        utilxx::overload{
            [](const UtilityTokenCreationOp&) {
                return forge::core::UTILITY_TOKEN_CREATION_FLAG;
            },
            [](const UtilityTokenDeletionOp&) {
                return forge::core::UTILITY_TOKEN_DELETION_FLAG;
            },
            [](const UtilityTokenOwnershipTransferOp&) {
                return forge::core::UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG;
            }},
        op);
}

auto forge::core::parseTransactionToUtilityTokenOp(Transaction tx,
                                                   std::int64_t block,
                                                   const ReadOnlyDaemonBase* daemon)
    -> utilxx::Result<utilxx::Opt<UtilityTokenOperation>,
                      DaemonError>
{
    using ResultType = utilxx::Result<utilxx::Opt<UtilityTokenOperation>,
                                      DaemonError>;

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
        tx.getFirstOpReturnOutput()
            .getValue()
            .get();

    //get optional new owner
    //for ownership transfer
    auto new_owner_opt =
        tx.getFirstNonOpReturnOutput()
            .flatMap([](auto ref)
                         -> utilxx::Opt<std::string> {
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
        .flatMap([&](auto resolvedVin) {
            //we can only have one input address
            if(resolvedVin.getAddresses().size() != 1) {
                return ResultType{std::nullopt};
            }

            //get owner
            auto owner = std::move(resolvedVin.getAddresses()[0]);

            //parse the metadata nd put it into
            //the ResultType
            return ResultType{
                parseMetadataToUtilityTokenOp(std::move(metadata),
                                              block,
                                              std::move(owner),
                                              value,
                                              std::move(new_owner_opt))};
        });
}


auto forge::core::parseMetadataToUtilityTokenOp(const std::vector<std::byte>& metadata,
                                                std::int64_t block,
                                                std::string&& owner,
                                                std::int64_t burn_value,
                                                utilxx::Opt<std::string>&& new_owner_opt)
    -> utilxx::Opt<UtilityTokenOperation>
{
    if(metadata.size() < 14) {
        return std::nullopt;
    }

    return parseUtilityToken(metadata)
        .flatMap([&](auto entry)
                     -> utilxx::Opt<UtilityTokenOperation> {
            auto amount = entry.getAttachedAmount();

            switch(metadata[OPERATION_FLAG_INDEX]) {

            case UTILITY_TOKEN_CREATION_FLAG:
                return UtilityTokenOperation{
                    UtilityTokenCreationOp{std::move(entry),
                                           amount,
                                           std::move(owner),
                                           block,
                                           burn_value}};

            case UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG:
                return new_owner_opt
                    .map([&](auto new_owner) {
                        return UtilityTokenOperation{
                            UtilityTokenOwnershipTransferOp{std::move(entry),
                                                            amount,
                                                            std::move(owner),
                                                            std::move(new_owner),
                                                            block,
                                                            burn_value}};
                    });
            case UTILITY_TOKEN_DELETION_FLAG:
                return UtilityTokenOperation{
                    UtilityTokenDeletionOp{std::move(entry),
                                           amount,
                                           std::move(owner),
                                           block,
                                           burn_value}};

            default:
                return std::nullopt;
            }
        });
}


auto forge::core::toMetadata(UtilityTokenOperation&& op)
    -> std::vector<std::byte>
{
    return std::visit(
        utilxx::overload{
            [](UtilityTokenCreationOp&& creation) {
                return createUtilityTokenCreationOpMetadata(
                    std::move(creation.getUtilityToken()));
            },
            [](UtilityTokenDeletionOp&& creation) {
                return createUtilityTokenDeletionOpMetadata(
                    std::move(creation.getUtilityToken()));
            },
            [](UtilityTokenOwnershipTransferOp&& creation) {
                return createUtilityTokenOwnershipTransferOpMetadata(
                    std::move(creation.getUtilityToken()));
            },
        },
        std::move(op));
}
