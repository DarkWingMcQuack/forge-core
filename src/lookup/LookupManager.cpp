#include <algorithm>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <client/ReadOnlyClientBase.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <fmt/format.h>
#include <functional>
#include <g3log/g3log.hpp>
#include <iterator>
#include <lookup/LookupManager.hpp>
#include <lookup/UMEntryLookup.hpp>
#include <memory>
#include <shared_mutex>
#include <utils/Opt.hpp>
#include <utils/Overload.hpp>
#include <utils/Result.hpp>

using forge::lookup::LookupManager;
using forge::lookup::LookupError;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::UMEntryOperation;
using forge::core::UniqueEntryOperation;
using forge::core::getMaturity;
using forge::utils::Opt;
using forge::utils::Result;
using forge::utils::traverse;
using forge::client::ReadOnlyClientBase;

LookupManager::LookupManager(std::unique_ptr<client::ReadOnlyClientBase>&& client)
    : client_(std::move(client)),
      rw_mtx_(std::make_unique<std::shared_mutex>()),
      um_entry_lookup_(this, getStartingBlock(client_->getCoin())),
      unique_entry_lookup_(this, getStartingBlock(client_->getCoin())),
      utility_token_lookup_(this, core::getStartingBlock(client_->getCoin()))
{}

auto LookupManager::updateLookup()
    -> utils::Result<bool, ManagerError>
{
    //aquire writer lock
    std::unique_lock lock{*rw_mtx_};
    const auto maturity = getMaturity(client_->getCoin());

    return client_->getBlockCount()
        .mapError([](auto error) {
            return ManagerError{std::move(error)};
        })
        .flatMap([&](auto actual_height)
                     -> Result<bool, ManagerError> {
            //if we have less blocks than maturity
            //then nothing happens
            if(actual_height < maturity) {
                LOG(DEBUG) << "not enough blocks to have any mature blocks";
                return false;
            }

            auto new_block_added{false};

            //process missing blocks
            while(actual_height - maturity > lookup_block_height_) {
                auto res =
                    //get block hash
                    client_->getBlockHash(++lookup_block_height_)
                        .flatMap([&](auto block_hash) {
                            //get block
                            return client_->getBlock(std::move(block_hash));
                        })
                        .mapError([](auto error) {
                            return ManagerError{std::move(error)};
                        })
                        //process the block
                        .flatMap([&](auto block) {
                            return processBlock(std::move(block));
                        });

                //if an error occured return the error
                if(!res) {
                    return res.getError();
                }

                new_block_added = true;
            }

            return new_block_added;
        });
}

auto LookupManager::rebuildLookup()
    -> utils::Result<void, ManagerError>
{
    std::unique_lock lock{*rw_mtx_};
    um_entry_lookup_.clear();
    unique_entry_lookup_.clear();
    lock.unlock();

    if(auto res = updateLookup();
       !res) {
        return res.getError();
    }

    return {};
}

auto LookupManager::lookupUMValue(const core::EntryKey& key) const
    -> utils::Opt<std::reference_wrapper<const core::UMEntryValue>>
{
    std::shared_lock lock{*rw_mtx_};
    return um_entry_lookup_.lookup(key);
}

auto LookupManager::lookupUniqueValue(const core::EntryKey& key) const
    -> utils::Opt<std::reference_wrapper<const core::UniqueEntryValue>>
{
    std::shared_lock lock{*rw_mtx_};
    return unique_entry_lookup_.lookup(key);
}


auto LookupManager::lookup(const core::EntryKey& key) const
    -> utils::Opt<core::Entry>
{
    if(auto um_value = lookupUMValue(key);
       um_value) {
        auto value = um_value.getValue().get();
        return core::Entry{
            core::UMEntry{key,
                          std::move(value)}};
    }
    if(auto unique_value = lookupUniqueValue(key);
       unique_value) {
        auto value = unique_value.getValue().get();
        return core::Entry{
            core::UniqueEntry{key,
                              std::move(value)}};
    }

    return std::nullopt;
}

auto LookupManager::lookupOwner(const core::EntryKey& key) const
    -> utils::Opt<std::reference_wrapper<const std::string>>
{
    std::shared_lock lock{*rw_mtx_};
    auto um_owner_opt = um_entry_lookup_.lookupOwner(key);

    if(um_owner_opt) {
        return um_owner_opt;
    }

    return unique_entry_lookup_.lookupOwner(key);
}

auto LookupManager::lookupActivationBlock(const core::EntryKey& key) const
    -> utils::Opt<std::reference_wrapper<const std::int64_t>>
{
    std::shared_lock lock{*rw_mtx_};

    auto um_block_op = um_entry_lookup_.lookupActivationBlock(key);
    if(um_block_op) {
        return um_block_op;
    }

    return unique_entry_lookup_.lookupActivationBlock(key);
}


auto LookupManager::processUMEntrys(const std::vector<core::Transaction>& txs,
                                    std::int64_t block_height)
    -> void
{
    //vector of all forge ops in the block
    std::vector<UMEntryOperation> ops;
    //exract all forge ops from the txs
    for(const auto& tx : txs) {
        const auto& txid = tx.getTxid();

        auto op_res = parseTransactionToUMEntry(tx,
                                                block_height,
                                                client_.get());
        //if we dont get an opt, but an error we log it
        if(!op_res) {
            LOG(WARNING) << op_res.getError().what();
            continue;
        }

        LOG_IF(DEBUG, op_res.getValue().hasValue())
            << "found unique modifiable entry operation "
            << txid;

        //check if the operation was parsed, and if
        //we add it to the std::vector<UMEntryOperation> vec
        if(auto op_opt = std::move(op_res.getValue());
           op_opt) {
            ops.push_back(std::move(op_opt.getValue()));
        }
    }

    um_entry_lookup_.executeOperations(std::move(ops));
}

auto LookupManager::processUniqueEntrys(const std::vector<core::Transaction>& txs,
                                        std::int64_t block_height)
    -> void
{
    //vector of all forge ops in the block
    std::vector<UniqueEntryOperation> ops;
    //exract all forge ops from the txs
    for(const auto& tx : txs) {
        const auto& txid = tx.getTxid();

        auto op_res = parseTransactionToUniqueEntry(tx,
                                                    block_height,
                                                    client_.get());
        //if we dont get an opt, but an error we log it
        if(!op_res) {
            LOG(WARNING) << op_res.getError().what();
            continue;
        }

        LOG_IF(DEBUG, op_res.getValue().hasValue())
            << "found unique immutable entry operation "
            << txid;

        //check if the operation was parsed, and if
        //we add it to the std::vector<UMEntryOperation> vec
        if(auto op_opt = std::move(op_res.getValue());
           op_opt) {
            ops.push_back(std::move(op_opt.getValue()));
        }
    }

    unique_entry_lookup_.executeOperations(std::move(ops));
}

auto LookupManager::processUtilityTokens(const std::vector<core::Transaction>& txs,
                                         std::int64_t block_height)
    -> void
{
    //vector of all forge ops in the block
    std::vector<core::UtilityTokenOperation> ops;
    //exract all forge ops from the txs
    for(const auto& tx : txs) {
        const auto& txid = tx.getTxid();

        auto op_res = core::parseTransactionToUtilityTokenOp(tx,
                                                             block_height,
                                                             client_.get());
        //if we dont get an opt, but an error we log it
        if(!op_res) {
            LOG(WARNING) << op_res.getError().what();
            continue;
        }

        LOG_IF(DEBUG, op_res.getValue().hasValue())
            << "found unique immutable entry operation "
            << txid;

        //check if the operation was parsed, and if
        //we add it to the std::vector<UMEntryOperation> vec
        if(auto op_opt = std::move(op_res.getValue());
           op_opt) {
            ops.push_back(std::move(op_opt.getValue()));
        }
    }

    utility_token_lookup_.executeOperations(std::move(ops));
}

auto LookupManager::processBlock(core::Block&& block)
    -> utils::Result<void, ManagerError>
{
    auto block_height = block.getHeight();
    auto block_hash = std::move(block.getHash());

    //traverse all txids to transactions
    auto txs_res =
        traverse(
            std::move(block.getTxids()),
            [this](auto txid) {
                return client_
                    ->getTransaction(std::move(txid))
                    .mapError([](auto error) {
                        return ManagerError{std::move(error)};
                    });
            });

    //check if an error occured
    if(!txs_res) {
        auto error = txs_res.getError();
        return ManagerError{std::move(error)};
    }

    //extract transactions
    auto transactions = txs_res.getValue();
    auto [um_ops, unique_ops, utility_ops] =
        parseAndFilter(std::move(transactions),
                       block_height);

    um_entry_lookup_.executeOperations(std::move(um_ops));
    unique_entry_lookup_.executeOperations(std::move(unique_ops));
    utility_token_lookup_.executeOperations(std::move(utility_ops));

    //add blockhash to the processed blocks
    block_hashes_.push_back(std::move(block_hash));

    return {};
}

auto LookupManager::lookupIsValid() const
    -> utils::Result<bool, client::ClientError>
{
    return getLastValidBlockHeight()
        .map([this](auto last_valid_block) {
            auto starting_block =
                getStartingBlock(client_->getCoin());

            return static_cast<std::int64_t>(block_hashes_.size())
                == last_valid_block - starting_block;
        });
}

auto LookupManager::getLastValidBlockHeight() const
    -> utils::Result<int64_t, client::ClientError>
{
    auto starting_block = getStartingBlock(client_->getCoin());

    std::shared_lock lock{*rw_mtx_};
    for(auto&& hash : block_hashes_) {
        if(auto res = client_->getBlockHash(++starting_block);
           res) {
            if(res.getValue() != hash) {
                return starting_block - 1;
            }
        } else {
            return res.getError();
        }
    }

    return starting_block;
}

auto LookupManager::getUMEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::UMEntry>
{
    std::shared_lock lock{*rw_mtx_};
    return um_entry_lookup_.getUMEntrysOfOwner(owner);
}


auto LookupManager::getUniqueEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::UniqueEntry>
{
    std::shared_lock lock{*rw_mtx_};
    return unique_entry_lookup_.getUniqueEntrysOfOwner(owner);
}

auto LookupManager::getUtilityTokensOfOwner(const std::string& owner) const
    -> std::vector<core::UtilityToken>
{
    std::shared_lock lock{*rw_mtx_};
    return utility_token_lookup_.getUtilityTokensOfOwner(owner);
}

auto LookupManager::getUtilityTokenCreditOf(const std::string& owner,
                                            const std::vector<std::byte>& token) const
    -> std::uint64_t
{
    std::shared_lock lock{*rw_mtx_};
    return utility_token_lookup_.getAvailableBalanceOf(owner,
                                                       token);
}

auto LookupManager::getSupplyOfToken(const std::vector<std::byte>& token) const
    -> std::uint64_t
{
    std::shared_lock lock{*rw_mtx_};
    return utility_token_lookup_.getSupplyOfToken(token);
}

auto LookupManager::getNumberOfExisitingTokens() const
    -> std::int64_t
{
    std::shared_lock lock{*rw_mtx_};
    return utility_token_lookup_.getNumberOfTokens();
}

auto LookupManager::getEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::Entry>
{
    auto unique = getUniqueEntrysOfOwner(owner);
    auto um = getUMEntrysOfOwner(owner);
    auto tokens = getUtilityTokensOfOwner(owner);

    std::vector<core::Entry> entrys;

    std::transform(std::make_move_iterator(std::begin(unique)),
                   std::make_move_iterator(std::end(unique)),
                   std::back_inserter(entrys),
                   [](auto entry) {
                       return core::Entry{std::move(entry)};
                   });

    std::transform(std::make_move_iterator(std::begin(um)),
                   std::make_move_iterator(std::end(um)),
                   std::back_inserter(entrys),
                   [](auto entry) {
                       return core::Entry{std::move(entry)};
                   });

    std::transform(std::make_move_iterator(std::begin(tokens)),
                   std::make_move_iterator(std::end(tokens)),
                   std::back_inserter(entrys),
                   [](auto entry) {
                       return core::Entry{std::move(entry)};
                   });

    return entrys;
}

auto LookupManager::isReserverdEntryKey(const std::vector<std::byte>& key) const
    -> bool
{
    auto unique_opt = unique_entry_lookup_.lookup(key);
    auto um_opt = um_entry_lookup_.lookup(key);
    auto utility_token_opt = utility_token_lookup_.getSupplyOfToken(key);

    return unique_opt || um_opt || utility_token_opt;
}

auto forge::lookup::generateMessage(ManagerError&& error)
    -> std::string
{
    static constexpr auto visitor = utils::overload{
        [](LookupError&& error) {
            return fmt::format("LookupError inside ManagerError: {}",
                               std::move(error.what()));
        },
        [](client::ClientError&& error) {
            return fmt::format("ClientError inside ManagerError: {}",
                               std::move(error.what()));
        }};

    return std::visit(visitor,
                      std::move(error));
}

auto LookupManager::getClient() const
    -> const client::ReadOnlyClientBase&
{
    return *client_;
}

auto LookupManager::getCoin() const
    -> core::Coin
{
    return client_->getCoin();
}


auto LookupManager::extractUMEntryOperations(const std::vector<core::Transaction>& txs,
                                             std::int64_t block_height)
    -> std::vector<core::UMEntryOperation>
{
    std::vector<core::UMEntryOperation> um_ops;

    for(const auto& tx : txs) {
        auto um_res = core::parseTransactionToUMEntry(tx, block_height, client_.get());
        if(!um_res) {
            //getting an error instead of an Opt indicates a wallet error
            LOG(WARNING) << um_res.getError().what();
            continue;
        }

        auto um_op_opt = std::move(um_res.getValue());
        if(!um_op_opt) {
            continue;
        }

        auto um_op = std::move(um_op_opt.getValue());
        um_ops.emplace_back(std::move(um_op));
    }

    return um_ops;
}

auto LookupManager::extractUniqueEntryOperations(const std::vector<core::Transaction>& txs,
                                                 std::int64_t block_height)
    -> std::vector<core::UniqueEntryOperation>
{
    std::vector<core::UniqueEntryOperation> unique_ops;

    for(const auto& tx : txs) {
        auto unique_res = core::parseTransactionToUniqueEntry(tx, block_height, client_.get());
        if(!unique_res) {
            //getting an error instead of an Opt indicates a wallet error
            LOG(WARNING) << unique_res.getError().what();
            continue;
        }

        auto unique_op_opt = std::move(unique_res.getValue());
        if(!unique_op_opt) {
            continue;
        }

        auto unique_op = std::move(unique_op_opt.getValue());
        unique_ops.emplace_back(std::move(unique_op));
    }

    return unique_ops;
}

auto LookupManager::extractUtilityTokenOperations(const std::vector<core::Transaction>& txs,
                                                  std::int64_t block_height)
    -> std::vector<core::UtilityTokenOperation>
{
    std::vector<core::UtilityTokenOperation> utility_ops;
    for(const auto& tx : txs) {
        auto utility_res = core::parseTransactionToUtilityTokenOp(tx, block_height, client_.get());
        if(!utility_res) {
            //getting an error instead of an Opt indicates a wallet error
            LOG(WARNING) << utility_res.getError().what();
            continue;
        }

        auto utility_op_opt = std::move(utility_res.getValue());
        if(!utility_op_opt) {
            continue;
        }

        auto utility_op = std::move(utility_op_opt.getValue());
        utility_ops.emplace_back(std::move(utility_op));
    }

    return utility_ops;
}

auto LookupManager::parseAndFilter(std::vector<core::Transaction>&& txs,
                                   std::int64_t block_height)
    -> std::tuple<std::vector<core::UMEntryOperation>,
                  std::vector<core::UniqueEntryOperation>,
                  std::vector<core::UtilityTokenOperation>>
{
    std::map<EntryKey, std::vector<core::EntryCreationOp>> creation_map;

    std::vector<core::UMEntryOperation> um_ops;
    std::vector<core::UniqueEntryOperation> unique_ops;
    std::vector<core::UtilityTokenOperation> utility_ops;

    auto raw_um_ops = extractUMEntryOperations(txs, block_height);
    auto raw_unique_ops = extractUniqueEntryOperations(txs, block_height);
    auto raw_utility_ops = extractUtilityTokenOperations(txs, block_height);

    for(auto um_op : std::move(raw_um_ops)) {
        if(std::holds_alternative<core::UMEntryCreationOp>(um_op)) {
            auto creation = std::get<core::UMEntryCreationOp>(std::move(um_op));
            auto key = creation.getEntryKey();
            auto [iter, inserted] = creation_map.insert({std::move(key), {creation}});
            if(!inserted) {
                iter->second.emplace_back(std::move(creation));
            }
            continue;
        }

        um_ops.emplace_back(std::move(um_op));
    }

    for(auto unique_op : std::move(raw_unique_ops)) {
        if(std::holds_alternative<core::UniqueEntryCreationOp>(unique_op)) {
            auto creation = std::get<core::UniqueEntryCreationOp>(std::move(unique_op));
            auto key = creation.getEntryKey();
            auto [iter, inserted] = creation_map.insert({std::move(key), {creation}});
            if(!inserted) {
                iter->second.emplace_back(std::move(creation));
            }
            continue;
        }

        unique_ops.emplace_back(std::move(unique_op));
    }

    for(auto utility_op : std::move(raw_utility_ops)) {
        if(std::holds_alternative<core::UtilityTokenCreationOp>(utility_op)) {
            auto creation = std::get<core::UtilityTokenCreationOp>(std::move(utility_op));
            auto key = creation.getUtilityToken().getId();
            auto [iter, inserted] = creation_map.insert({std::move(key), {creation}});
            if(!inserted) {
                iter->second.emplace_back(std::move(creation));
            }
            continue;
        }

        utility_ops.emplace_back(std::move(utility_op));
    }

    for(auto [_, creations] : std::move(creation_map)) {

        if(creations.empty()) {
            continue;
        }


        std::sort(std::begin(creations),
                  std::end(creations),
                  [](const auto& lhs, const auto& rhs) {
                      return core::getBurnValue(lhs) < core::getBurnValue(rhs);
                  });

        std::reverse(std::begin(creations),
                     std::end(creations));


        if(creations.size() == 1
           || core::getBurnValue(creations[0]) != core::getBurnValue(creations[1])) {
            std::visit(utils::overload{
                           [&](core::UMEntryCreationOp op) {
                               um_ops.emplace_back(std::move(op));
                           },
                           [&](core::UniqueEntryCreationOp op) {
                               unique_ops.emplace_back(std::move(op));
                           },
                           [&](core::UtilityTokenCreationOp op) {
                               utility_ops.emplace_back(std::move(op));
                           }},
                       std::move(creations[0]));
        }
    }

    return std::tuple{std::move(um_ops),
                      std::move(unique_ops),
                      std::move(utility_ops)};
}
