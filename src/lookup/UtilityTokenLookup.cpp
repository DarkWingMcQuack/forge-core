#include "core/Transaction.hpp"
#include "entrys/token/UtilityToken.hpp"
#include "entrys/token/UtilityTokenCreationOp.hpp"
#include "entrys/token/UtilityTokenDeletionOp.hpp"
#include "entrys/token/UtilityTokenOperation.hpp"
#include "entrys/token/UtilityTokenOwnershipTransferOp.hpp"
#include "utilxx/Overload.hpp"
#include <algorithm>
#include <cstdint>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <iterator>
#include <limits>
#include <lookup/UtilityTokenLookup.hpp>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

using forge::lookup::UtilityTokenLookup;
using forge::core::UtilityToken;
using forge::core::UtilityTokenOperation;
using forge::core::UtilityTokenCreationOp;
using forge::core::UtilityTokenDeletionOp;
using forge::core::UtilityTokenOwnershipTransferOp;


UtilityTokenLookup::UtilityTokenLookup(std::int64_t start_block)
    : block_height_(start_block),
      start_block_(start_block) {}

auto UtilityTokenLookup::executeOperations(std::vector<UtilityTokenOperation>&& ops)
    -> void
{
    auto filtered = filterNonRelevantOperations(std::move(ops));
    for(auto&& op : filtered) {
        std::visit(*this,
                   std::move(op));
    }
}

auto UtilityTokenLookup::setBlockHeight(std::int64_t height)
    -> void
{
    block_height_ = height;
}

auto UtilityTokenLookup::getBlockHeight() const
    -> std::int64_t
{
    return block_height_;
}

auto UtilityTokenLookup::getAvailableBalanceOf(const std::string& owner,
                                               const std::string& token) const
    -> std::uint64_t
{
    auto first_iter = utility_account_lookup_.find(token);
    if(first_iter == utility_account_lookup_.end()) {
        return 0;
    }

    const auto& [_, accounts] = *first_iter;
    auto second_iter = accounts.find(owner);
    if(second_iter == accounts.end()) {
        return 0;
    }

    return second_iter->second;
}

auto UtilityTokenLookup::clear()
    -> void
{
    utility_account_lookup_.clear();
    block_height_ = start_block_;
}

auto UtilityTokenLookup::getUtilityTokensOfOwner(std::string_view owner) const
    -> std::vector<std::pair<UtilityToken,
                             std::uint64_t>>
{
    std::vector<std::pair<UtilityToken,
                          std::uint64_t>>
        ret_vec;
    for(const auto& [token, accounts] : utility_account_lookup_) {
        for(const auto& [creditor, credit] : accounts) {
            if(creditor == owner) {
                auto data_opt = forge::core::stringToByteVec(token);
                if(!data_opt) {
                    LOG(WARNING) << "unable to convert utility token id"
                                 << token
                                 << " to byte vector";
                    continue;
                }
                auto data = std::move(data_opt.getValue());
                UtilityToken utility_token{std::move(data)};

                ret_vec.emplace_back(std::move(utility_token),
                                     credit);
            }
        }
    }

    return ret_vec;
}

auto UtilityTokenLookup::getNumberOfTokens() const
    -> std::int64_t
{
    return utility_account_lookup_.size();
}

auto UtilityTokenLookup::getSupplyOfToken(std::string_view token) const
    -> std::uint64_t
{
    auto iter = utility_account_lookup_.find(token.data());
    if(iter == std::end(utility_account_lookup_)) {
        return 0;
    }

    const auto& token_accs = iter->second;

    return std::accumulate(std::cbegin(token_accs),
                           std::cend(token_accs),
                           0,
                           [](auto init, const auto& acc) {
                               return init + acc.second;
                           });
}


auto UtilityTokenLookup::operator()(UtilityTokenCreationOp&& op)
    -> void
{
    auto creator = std::move(op.getCreator());
    auto amount = op.getAmount();
    auto raw_id = op.getUtilityToken().getId();
    auto id = forge::core::toHexString(raw_id);

    UtilityTokenAccounts account;
    account.emplace(std::move(creator),
                    amount);

    utility_account_lookup_.emplace(std::move(id),
                                    std::move(account));
}

auto UtilityTokenLookup::operator()(UtilityTokenOwnershipTransferOp&& op)
    -> void
{
    auto sender = std::move(op.getSender());
    auto reciever = std::move(op.getReciever());
    auto amount = std::move(op.getAmount());
    auto raw_id = std::move(op.getUtilityToken().getId());
    auto id = forge::core::toHexString(raw_id);

    auto& credit = utility_account_lookup_[id][sender];
    if(credit - amount == 0) {
        utility_account_lookup_[id].erase(sender);
    } else {
        credit -= amount;
    }

    if(auto iter = utility_account_lookup_[id].find(reciever);
       iter != utility_account_lookup_[id].end()) {
        iter->second += amount;
    } else {
        utility_account_lookup_[id].emplace(std::move(reciever),
                                            amount);
    }
}

auto UtilityTokenLookup::operator()(UtilityTokenDeletionOp&& op)
    -> void
{
    auto creator = std::move(op.getCreator());
    auto amount = std::move(op.getAmount());
    auto raw_id = std::move(op.getUtilityToken().getId());
    auto id = forge::core::toHexString(raw_id);

    auto& credit = utility_account_lookup_[id][creator];
    if(credit - amount == 0) {
        utility_account_lookup_[id].erase(creator);
    } else {
        credit -= amount;
    }

    if(utility_account_lookup_[id].empty()) {
        utility_account_lookup_.erase(id);
    }
}

auto UtilityTokenLookup::filterNonRelevantOperations(std::vector<UtilityTokenOperation>&& ops) const
    -> std::vector<UtilityTokenOperation>
{
    auto grouped = groupOperations(std::move(ops));
    std::vector<UtilityTokenOperation> relevant_ops;

    for(auto&& [id, operations] : grouped) {
        auto rel_ops = filterOperationsPerToken(id,
                                                std::move(operations));
        std::move(std::begin(rel_ops),
                  std::end(rel_ops),
                  std::back_inserter(relevant_ops));
    }

    return relevant_ops;
}


auto UtilityTokenLookup::filterOperationsPerToken(const std::string& token_id,
                                                  std::vector<UtilityTokenOperation>&& ops) const
    -> std::vector<UtilityTokenOperation>
{
    std::vector<UtilityTokenCreationOp> creations;
    std::vector<UtilityTokenDeletionOp> deletions;
    std::vector<UtilityTokenOwnershipTransferOp> ownership_transfers;

    for(auto&& op : ops) {
        std::visit(
            utilxx::overload{
                [&](UtilityTokenCreationOp&& creation) {
                    creations.emplace_back(std::move(creation));
                },
                [&](UtilityTokenDeletionOp&& deletion) {
                    deletions.emplace_back(std::move(deletion));
                },
                [&](UtilityTokenOwnershipTransferOp&& transfer) {
                    ownership_transfers.emplace_back(std::move(transfer));
                }},
            std::move(op));
    }

    //creations are only valid if the token does not already exist
    //on the other hand deletions and transfers are only valid
    //it the token already exists
    if(checkIfTokenExists(token_id)) {
        creations.clear();
    } else {
        deletions.clear();
        ownership_transfers.clear();

        //return the creation op with the highest burn value
        auto iter =
            std::max_element(std::cbegin(creations),
                             std::cend(creations),
                             [](const auto& lhs, const auto& rhs) {
                                 return lhs.getBurnValue() < rhs.getBurnValue();
                             });

        if(iter == std::cend(creations)) {
            return {};
        }

        return {std::move(*iter)};
    }

    //collect how much users have spend within all the transactions
    std::unordered_map<std::string_view,
                       std::uint64_t>
        used_balance;

    //overflow set contains all creators who have overflown
    //a std::uint64_t with their expenses
    std::unordered_set<std::string_view>
        is_overflow;

    //fill overflown set and used_balance for all deletions
    for(const auto& op : deletions) {
        const auto& creator = op.getCreator();
        auto new_added = op.getAmount();

        if(auto iter = used_balance.find(creator);
           iter != used_balance.end()) {
            auto& current = iter->second;

            //if an overflow occurs then we add
            // the creator to the overflow set
            if(!isSaveAddition(current, new_added)) {
                is_overflow.insert(creator);
            }

            current += new_added;

        } else {
            used_balance.emplace(creator,
                                 new_added);
        }
    }

    //fill overflown set and used_balance for all token transfers
    for(const auto& op : ownership_transfers) {

        const auto& sender = op.getSender();
        auto new_added = op.getAmount();

        if(auto iter = used_balance.find(sender);
           iter != used_balance.end()) {
            auto& current = iter->second;

            //if an overflow occurs then we add
            // the creator to the overflow set
            if(!isSaveAddition(current, new_added)) {
                is_overflow.insert(sender);
            }

            current += new_added;

        } else {
            used_balance.emplace(sender,
                                 new_added);
        }
    }

    //erase all operations from users
    //which have spend more than they have in total
    //or have overflown a std::uint64_t
    ownership_transfers
        .erase(
            std::remove_if(std::begin(ownership_transfers),
                           std::end(ownership_transfers),
                           [&](const auto& creat) {
                               const auto& sender = creat.getSender();

                               //if the transaction is created by a sender
                               //who has overflown a std::uint64_t in this block
                               //delete its fuckin transaction
                               if(is_overflow.find(sender) != is_overflow.end()) {
                                   return true;
                               }

                               //otherwise check if he has enought credit to
                               //perform all of his transactions
                               auto used = used_balance[sender];
                               auto available = getAvailableBalanceOf(sender,
                                                                      token_id);

                               return used > available;
                           }),
            std::end(ownership_transfers));

    deletions
        .erase(
            std::remove_if(std::begin(deletions),
                           std::end(deletions),
                           [&](const auto& creat) {
                               const auto& creator = creat.getCreator();

                               //if the deletion is created by an owner
                               //who has overflown a std::uint64_t in this block
                               //delete its fuckin transaction
                               if(is_overflow.find(creator) != is_overflow.end()) {
                                   return true;
                               }

                               //otherwise check if he has enought credit to
                               //perform all of his transactions
                               auto used = used_balance[creator];
                               auto available = getAvailableBalanceOf(creator,
                                                                      token_id);

                               return used > available;
                           }),
            std::end(deletions));

    std::vector<UtilityTokenOperation> ret_ops;

    std::move(std::begin(ownership_transfers),
              std::end(ownership_transfers),
              std::back_inserter(ret_ops));
    std::move(std::begin(deletions),
              std::end(deletions),
              std::back_inserter(ret_ops));

    return ret_ops;
}

auto UtilityTokenLookup::groupOperations(std::vector<UtilityTokenOperation>&& ops) const
    -> std::unordered_map<std::string,
                          std::vector<UtilityTokenOperation>>
{
    std::unordered_map<std::string,
                       std::vector<UtilityTokenOperation>>
        operations;

    for(auto&& op : ops) {
        std::visit(
            [&](auto&& operation) {
                const auto& token_id = operation.getUtilityToken().getId();
                auto id_str = forge::core::toHexString(token_id);
                auto iter = operations.find(id_str);
                if(iter != operations.end()) {
                    iter
                        ->second
                        .emplace_back(std::move(operation));
                } else {
                    UtilityTokenOperation op{std::move(operation)};
                    operations.emplace(std::move(id_str),
                                       std::vector{std::move(op)});
                }
            },
            std::move(op));
    }

    return operations;
}

auto UtilityTokenLookup::checkIfTokenExists(const std::string& token_id) const
    -> bool
{
    return utility_account_lookup_.find(token_id)
        != utility_account_lookup_.end();
}


auto forge::lookup::isSaveAddition(std::uint64_t first,
                                   std::uint64_t second)
    -> bool

{
    return std::numeric_limits<std::uint64_t>::max()
        - second
        >= first;
}
