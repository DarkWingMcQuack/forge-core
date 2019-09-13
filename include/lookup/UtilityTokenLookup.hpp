#pragma once

#include <core/Coin.hpp>
#include <cstddef>
#include <cstdint>
#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <lookup/LookupError.hpp>
#include <map>
#include <string_view>
#include <unordered_map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>
#include <vector>

namespace forge::lookup {

class LookupManager;

class UtilityTokenLookup final
{
public:
    UtilityTokenLookup(const LookupManager* const manager, std::int64_t start_block = 0);

    //filters out operations which would be illegal and then executes them
    auto executeOperations(std::vector<core::UtilityTokenOperation>&& ops)
        -> void;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    //returns available balance of the token for a owner
    auto getAvailableBalanceOf(const std::string& owner,
                               const std::vector<std::byte>& token) const
        -> std::uint64_t;

    //resets the lookup
    auto clear()
        -> void;

    //returns a pairs (token, balance) for all tokens an owner owns
    auto getUtilityTokensOfOwner(std::string_view owner) const
        -> std::vector<core::UtilityToken>;

    auto getNumberOfTokens() const
        -> std::int64_t;

    auto getSupplyOfToken(const std::vector<std::byte>& token) const
        -> std::uint64_t;

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
    auto filterOperationsPerToken(const std::vector<std::byte>& token_id,
                                  std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;

    //returns true if a token with a given id exists, false otherwise
    auto checkIfTokenExists(const std::vector<std::byte>& token_id) const
        -> bool;

    //groups the transactions according to which token they refer
    auto groupOperationsByToken(std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::map<std::vector<std::byte>,
                    std::vector<core::UtilityTokenOperation>>;

    auto groupOperationsByCreator(std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::unordered_map<std::string,
                              std::vector<
                                  core::UtilityTokenOperation>>;

    //extracts the relevant operations for a user based on his balance
    //it is assumed that all operations are transfers or deletions
    //and that all operations have the same creator
    auto extractRelevantOperations(const std::string& creator,
                                   const std::vector<std::byte>& token,
                                   std::vector<core::UtilityTokenOperation>&& ops) const
        -> std::vector<core::UtilityTokenOperation>;



private:
    using UtilityTokenAccounts =
        std::unordered_map<std::string, //owner
                           std::uint64_t>; //number of owned tokens

    std::map<std::vector<std::byte>, // token id
             UtilityTokenAccounts> //token accounts
        utility_account_lookup_;

    const LookupManager* const manager_;
    std::int64_t block_height_;
    std::int64_t start_block_;
};

//returns true if the addition of the two given integers would overfow,
//false if not
auto isSaveAddition(std::uint64_t first,
                    std::uint64_t second)
    -> bool;

} // namespace forge::lookup
