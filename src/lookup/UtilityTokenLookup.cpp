#include "core/Transaction.hpp"
#include "entrys/token/UtilityToken.hpp"
#include "entrys/token/UtilityTokenCreationOp.hpp"
#include "entrys/token/UtilityTokenDeletionOp.hpp"
#include "entrys/token/UtilityTokenOperation.hpp"
#include "entrys/token/UtilityTokenOwnershipTransferOp.hpp"
#include "utilxx/Overload.hpp"
#include <algorithm>
#include <boost/type_traits/is_destructible.hpp>
#include <cstdint>
#include <g3log/g3log.hpp>
#include <iterator>
#include <lookup/UtilityTokenLookup.hpp>
#include <unordered_map>

using forge::lookup::UtilityTokenLookup;


UtilityTokenLookup::UtilityTokenLookup(std::int64_t start_block)
    : block_height_(start_block),
      start_block_(start_block) {}

auto UtilityTokenLookup::executeOperations(std::vector<core::UtilityTokenOperation>&& ops)
    -> void
{
    for(auto&& op : ops) {
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
    -> std::vector<std::pair<core::UtilityToken,
                             std::uint64_t>>
{
    std::vector<std::pair<core::UtilityToken,
                          std::uint64_t>>
        ret_vec;
    for(const auto& [token, accounts] : utility_account_lookup_) {
        for(const auto& [creditor, credit] : accounts) {
            if(creditor == owner) {
                auto data_opt = core::stringToByteVec(token);
                if(!data_opt) {
                    LOG(WARNING) << "unable to convert utility token id"
                                 << token
                                 << " to byte vector";
                    continue;
                }
                auto data = std::move(data_opt.getValue());
                core::UtilityToken utility_token{std::move(data)};

                ret_vec.emplace_back(std::move(utility_token),
                                     credit);
            }
        }
    }

    return ret_vec;
}

auto UtilityTokenLookup::operator()(core::UtilityTokenCreationOp&& op)
    -> void
{
    auto creator = std::move(op.getCreator());
    auto amount = op.getAmount();
    auto raw_id = op.getUtilityToken().getId();
    auto id = core::toHexString(raw_id);

    UtilityTokenAccounts account;
    account.emplace(std::move(creator),
                    amount);

    utility_account_lookup_.emplace(std::move(id),
                                    std::move(account));
}

auto UtilityTokenLookup::operator()(core::UtilityTokenOwnershipTransferOp&& op)
    -> void
{
    auto sender = std::move(op.getSender());
    auto reciever = std::move(op.getReciever());
    auto amount = std::move(op.getAmount());
    auto raw_id = std::move(op.getUtilityToken().getId());
    auto id = core::toHexString(raw_id);

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

auto UtilityTokenLookup::operator()(core::UtilityTokenDeletionOp&& op)
    -> void
{
    auto creator = std::move(op.getCreator());
    auto amount = std::move(op.getAmount());
    auto raw_id = std::move(op.getUtilityToken().getId());
    auto id = core::toHexString(raw_id);

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

auto UtilityTokenLookup::filterNonRelevantOperations(std::vector<core::UtilityTokenOperation>&& ops) const
    -> std::vector<core::UtilityTokenOperation>
{
    auto grouped = groupOperations(std::move(ops));
    std::vector<core::UtilityTokenOperation> relevant_ops;

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
                                                  std::vector<core::UtilityTokenOperation>&& ops) const
    -> std::vector<core::UtilityTokenOperation>
{
    std::vector<core::UtilityTokenCreationOp> creations;
    std::vector<core::UtilityTokenDeletionOp> deletions;
    std::vector<core::UtilityTokenOwnershipTransferOp> ownership_transfers;

    for(auto&& op : ops) {
        std::visit(
            utilxx::overload{
                [&](core::UtilityTokenCreationOp&& creation) {
                    creations.emplace_back(std::move(creation));
                },
                [&](core::UtilityTokenDeletionOp&& deletion) {
                    deletions.emplace_back(std::move(deletion));
                },
                [&](core::UtilityTokenOwnershipTransferOp&& transfer) {
                    ownership_transfers.emplace_back(std::move(transfer));
                }},
            std::move(op));
    }

    if(checkIfTokenExists(token_id)) {
        creations.clear();
    } else {
        deletions.clear();
        ownership_transfers.clear();

        //return the creation op with the highest burn value
        auto iter = std::max_element(std::cbegin(creations),
                                     std::cend(creations),
                                     [](const auto& lhs, const auto& rhs) {
                                         return lhs.getBurnValue() < rhs.getBurnValue();
                                     });

        if(iter == std::cend(creations)) {
            return {};
        } else {
            return std::vector<core::UtilityTokenOperation>{std::move(*iter)};
        }
    }

    //collect how much users have spend within all the transactions
    std::unordered_map<std::string_view,
                       std::uint64_t>
        used_balance;

    for(const auto& op : deletions) {
        auto iter = used_balance.find(op.getCreator());
        if(iter != used_balance.end()) {
            iter->second += op.getAmount();
        } else {
            used_balance.emplace(op.getCreator(),
                                 op.getAmount());
        }
    }

    //erase all operations from users
    //which have spend more than they have in total
    ownership_transfers
        .erase(
            std::remove_if(std::begin(ownership_transfers),
                           std::end(ownership_transfers),
                           [&](const auto& creat) {
                               const auto& sender = creat.getSender();
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
                               auto used = used_balance[creator];
                               auto available = getAvailableBalanceOf(creator,
                                                                      token_id);

                               return used > available;
                           }),
            std::end(deletions));

    std::vector<core::UtilityTokenOperation> ret_ops;

    std::move(std::begin(ownership_transfers),
              std::end(ownership_transfers),
              std::back_inserter(ret_ops));
    std::move(std::begin(deletions),
              std::end(deletions),
              std::back_inserter(ret_ops));

    return ret_ops;
}

auto UtilityTokenLookup::groupOperations(std::vector<core::UtilityTokenOperation>&& ops) const
    -> std::unordered_map<std::string,
                          std::vector<core::UtilityTokenOperation>>
{
    std::unordered_map<std::string,
                       std::vector<core::UtilityTokenOperation>>
        operations;

    for(auto&& op : ops) {
        std::visit(
            [&](auto&& operation) {
                const auto& token_id = operation.getUtilityToken().getId();
                auto id_str = core::toHexString(token_id);
                auto iter = operations.find(id_str);
                if(iter != operations.end()) {
                    iter
                        ->second
                        .emplace_back(std::move(operation));
                } else {
                    core::UtilityTokenOperation op{std::move(operation)};
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

