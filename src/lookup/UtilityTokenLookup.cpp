#include "core/Transaction.hpp"
#include "entrys/token/UtilityToken.hpp"
#include "entrys/token/UtilityTokenCreationOp.hpp"
#include "entrys/token/UtilityTokenDeletionOp.hpp"
#include "entrys/token/UtilityTokenOperation.hpp"
#include "entrys/token/UtilityTokenOwnershipTransferOp.hpp"
#include "utilxx/Overload.hpp"
#include <algorithm>
#include <cstdint>
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
    -> std::vector<UtilityToken>
{
    std::vector<UtilityToken> ret_vec;

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
                UtilityToken utility_token{std::move(data),
                                           credit};

                ret_vec.emplace_back(std::move(utility_token));
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
    auto sender = std::move(op.getCreator());
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
    auto grouped = groupOperationsByToken(std::move(ops));
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
    std::vector<UtilityTokenOperation> changing_ops;

    for(auto&& op : ops) {
        std::visit(
            utilxx::overload{
                [&](UtilityTokenCreationOp&& creation) {
                    if(creation.getAmount() != 0) {
                        creations.emplace_back(std::move(creation));
                    }
                },
                [&](auto change) {
                    if(change.getAmount() != 0) {
                        changing_ops.emplace_back(std::move(change));
                    }
                }},
            std::move(op));
    }

    //creations are only valid if the token does not already exist
    //on the other hand deletions and transfers are only valid
    //it the token already exists
    if(checkIfTokenExists(token_id)) {
        creations.clear();
    } else {
        changing_ops.clear();

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

    auto grouped = groupOperationsByCreator(std::move(changing_ops));
    changing_ops.clear();

    std::vector<UtilityTokenOperation> ret_ops;
    for(auto&& [creator, operations] : grouped) {
        auto valid_ops =
            extractRelevantOperations(creator,
                                      token_id,
                                      std::move(operations));
        std::move(std::begin(valid_ops),
                  std::end(valid_ops),
                  std::back_inserter(ret_ops));
    }

    return ret_ops;
}

auto UtilityTokenLookup::extractRelevantOperations(const std::string& creator,
                                                   const std::string& token,
                                                   std::vector<core::UtilityTokenOperation>&& ops) const
    -> std::vector<core::UtilityTokenOperation>
{
    //sort by burn value
    //the operations with the highes burnvalue have the highest prioritys
    std::sort(std::begin(ops),
              std::end(ops),
              [](const auto& lhs_v, const auto& rhs_v) {
                  return std::visit(
                      [](const auto& lhs, const auto& rhs) {
                          return lhs.getBurnValue() > rhs.getBurnValue();
                      },
                      lhs_v,
                      rhs_v);
              });

    std::vector<UtilityTokenOperation> ret_vec;
    std::uint64_t used{0};
    auto available_balance = getAvailableBalanceOf(creator, token);

    for(auto&& op : ops) {
        auto new_added = std::visit(
            [](const auto& operation) {
                return operation.getAmount();
            },
            op);

        //if a user trys to overflow his used amount no
        //operation of him will be executed in this block
        if(!isSaveAddition(used, new_added)) {
            return {};
        }

        used += new_added;

        if(used > available_balance) {
            break;
        }

        ret_vec.emplace_back(std::move(op));
    }

    return ret_vec;
}


auto UtilityTokenLookup::groupOperationsByToken(std::vector<UtilityTokenOperation>&& ops) const
    -> std::unordered_map<std::string,
                          std::vector<UtilityTokenOperation>>
{
    std::unordered_map<std::string,
                       std::vector<UtilityTokenOperation>>
        operations;

    for(auto&& op : ops) {
        std::visit(
            [&](auto operation) {
                const auto& token_id = operation.getUtilityToken().getId();
                auto id_str = forge::core::toHexString(token_id);
                auto iter = operations.find(id_str);
                if(iter != operations.end()) {
                    iter
                        ->second
                        .emplace_back(std::move(operation));
                } else {
                    UtilityTokenOperation tmp_op{std::move(operation)};
                    operations.emplace(std::move(id_str),
                                       std::vector{std::move(tmp_op)});
                }
            },
            std::move(op));
    }

    return operations;
}

auto UtilityTokenLookup::groupOperationsByCreator(std::vector<core::UtilityTokenOperation>&& ops) const
    -> std::unordered_map<std::string,
                          std::vector<UtilityTokenOperation>>
{
    std::unordered_map<std::string,
                       std::vector<UtilityTokenOperation>>
        operations;

    for(auto&& op : ops) {
        std::visit(
            [&](auto operation) {
                const auto& creator = operation.getCreator();
                auto iter = operations.find(creator);
                if(iter != operations.end()) {
                    iter
                        ->second
                        .emplace_back(std::move(operation));
                } else {
                    auto creator_copy = creator;
                    UtilityTokenOperation tmp_op{std::move(operation)};
                    operations.emplace(creator_copy,
                                       std::vector{std::move(tmp_op)});
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
