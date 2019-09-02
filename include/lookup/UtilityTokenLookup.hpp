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

    //filters out operations which would be illegal and then executes them
    auto executeOperations(std::vector<core::UtilityTokenOperation>&& ops)
        -> void;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    //returns available balance of the token for a owner
    auto getAvailableBalanceOf(const std::string& owner,
                               const std::string& token) const
        -> std::uint64_t;

    //resets the lookup
    auto clear()
        -> void;

    //returns a pairs (token, balance) for all tokens an owner owns
    auto getUtilityTokensOfOwner(std::string_view owner) const
        -> std::vector<
            std::pair<core::UtilityToken,
                      std::uint64_t>>;

    //execute Creation Operation
    auto operator()(core::UtilityTokenCreationOp&& op)
        -> void;

    //execute OwnershipTransfers Operation
    auto operator()(core::UtilityTokenOwnershipTransferOp&& op)
        -> void;

    //execute Deletion Operation
    auto operator()(core::UtilityTokenDeletionOp&& op)
        -> void;

private:
    //throws away operations which would be illegal
    auto filterNonRelevantOperations(std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;

    //expects operations of exactly one token
    //filters out operations which would be illegal
    //like creating operations on a already existing token,
    //transfers/deletions of tokens which do nit exist
    //or transfers/deletions which would spend more than
    //the sender ownes
    auto filterOperationsPerToken(const std::string& token_id,
                                  std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;

    //returns true if a token with a given id exists, false otherwise
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