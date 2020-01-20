#pragma once

#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <cstdint>
#include <client/ReadOnlyClientBase.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/token/UtilityTokenDeletionOp.hpp>
#include <entrys/token/UtilityTokenOwnershipTransferOp.hpp>
#include <json/value.h>
#include <utils/Opt.hpp>
#include <variant>
#include <vector>

namespace forge::core {
// clang-format off
constexpr inline auto UTILITY_TOKEN_CREATION_FLAG     = static_cast<std::byte>(0b00000001);
constexpr inline auto UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG = static_cast<std::byte>(0b00000010);
constexpr inline auto UTILITY_TOKEN_DELETION_FLAG     = static_cast<std::byte>(0b00000100);
// clang-format on

using UtilityTokenOperation = std::variant<UtilityTokenCreationOp,
                                           UtilityTokenDeletionOp,
                                           UtilityTokenOwnershipTransferOp>;

auto getUtilitToken(const UtilityTokenOperation& op)
    -> const UtilityToken&;
auto getUtilityToken(UtilityTokenOperation&& op)
    -> UtilityToken;

auto getAmount(const UtilityTokenOperation& op)
    -> std::uint64_t;

//extracts owner of the creator UtilityTokenOp from a given operaton
auto getCreator(const UtilityTokenOperation&)
    -> const std::string&;
auto getCreator(UtilityTokenOperation &&)
    -> std::string;

//extracts the operation flag which specifys which operation it is
auto extractOperationFlag(const UtilityTokenOperation&)
    -> std::byte;


//checks the metadata of a transaction and parses it into
//an UtilityTokenOp if it holds the needed information
//and the metadata has the needed formating
auto parseTransactionToUtilityTokenOp(core::Transaction tx,
                                      std::int64_t block,
                                      const client::ReadOnlyClientBase* client)
    -> utils::Result<utils::Opt<UtilityTokenOperation>,
                      client::ClientError>;

//parses given metadata and constructs a UtilityTokenOp from
//the given information if possible
auto parseMetadataToUtilityTokenOp(const std::vector<std::byte>& metadata,
                                   std::int64_t block,
                                   std::string&& owner,
                                   std::int64_t value,
                                   utils::Opt<std::string>&& new_owner = std::nullopt)
    -> utils::Opt<UtilityTokenOperation>;

auto toMetadata(UtilityTokenOperation&& op)
    -> std::vector<std::byte>;

} // namespace forge::core
