#pragma once

#include <core/Coin.hpp>
#include <cstdint>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <lookup/LookupError.hpp>
#include <map>
#include <string_view>
#include <unordered_map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::lookup {

class UtilityTokenLookup final
{
public:
    UtilityTokenLookup(std::int64_t start_block = 0);

    auto executeOperations(std::vector<core::UtilityTokenOperation>&& ops)
        -> void;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    auto getAvailableBalanceOf(const std::string& owner,
                               const std::string& token) const
        -> std::uint64_t;

    auto clear()
        -> void;

    //returns a pairs (token, balance) for all tokens an owner owns
    auto getUtilityTokensOfOwner(std::string_view owner) const
        -> std::vector<
            std::pair<core::UtilityToken,
                      std::uint64_t>>;

    auto operator()(core::UtilityTokenCreationOp&& op)
        -> void;

    auto operator()(core::UtilityTokenOwnershipTransferOp&& op)
        -> void;

    auto operator()(core::UtilityTokenDeletionOp&& op)
        -> void;

private:
    auto filterNonRelevantOperations(std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;


    auto filterOperationsPerToken(const std::string& token_id,
                                  std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;

    auto checkIfTokenExists(const std::string& token_id) const
        -> bool;

    //groups the transactions according to which token they refer
    auto groupOperations(std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::unordered_map<std::string,
                              std::vector<
                                  core::UtilityTokenOperation>>;



private:
    using UtilityTokenAccounts =
        std::unordered_map<std::string, //owner
                           std::uint64_t>; //number of owned tokens

    std::unordered_map<std::string, // token id
                       UtilityTokenAccounts> //token accounts
        utility_account_lookup_;

    std::int64_t block_height_;
    std::int64_t start_block_;
};

} // namespace forge::lookup
