#include "entrys/token/UtilityToken.hpp"
#include "entrys/token/UtilityTokenOperation.hpp"
#include <algorithm>
#include <core/Coin.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <fmt/core.h>
#include <functional>
#include <g3log/g3log.hpp>
#include <iterator>
#include <lookup/LookupManager.hpp>
#include <lookup/UMEntryLookup.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <utilxx/Result.hpp>

using forge::lookup::LookupManager;
using forge::lookup::LookupError;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::UMEntryOperation;
using forge::core::UniqueEntryOperation;
using forge::core::getMaturity;
using utilxx::Opt;
using utilxx::Result;
using utilxx::traverse;
using forge::daemon::ReadOnlyDaemonBase;

LookupManager::LookupManager(std::unique_ptr<daemon::ReadOnlyDaemonBase>&& daemon)
    : daemon_(std::move(daemon)),
      um_entry_lookup_(getStartingBlock(daemon_->getCoin()))
{}

auto LookupManager::updateLookup()
    -> utilxx::Result<bool, ManagerError>
{
    //aquire writer lock
    std::unique_lock lock{rw_mtx_};
    const auto maturity = getMaturity(daemon_->getCoin());

    return daemon_->getBlockCount()
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
                    daemon_->getBlockHash(++lookup_block_height_)
                        .flatMap([&](auto block_hash) {
                            //get block
                            return daemon_->getBlock(std::move(block_hash));
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
    -> utilxx::Result<void, ManagerError>
{
    std::unique_lock lock{rw_mtx_};
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
    -> utilxx::Opt<std::reference_wrapper<const core::UMEntryValue>>
{
    std::shared_lock lock{rw_mtx_};
    return um_entry_lookup_.lookup(key);
}

auto LookupManager::lookupUniqueValue(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const core::UniqueEntryValue>>
{
    std::shared_lock lock{rw_mtx_};
    return unique_entry_lookup_.lookup(key);
}


auto LookupManager::lookup(const core::EntryKey& key) const
    -> utilxx::Opt<core::Entry>
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
    -> utilxx::Opt<std::reference_wrapper<const std::string>>
{
    std::shared_lock lock{rw_mtx_};
    auto um_owner_opt = um_entry_lookup_.lookupOwner(key);

    if(um_owner_opt) {
        return um_owner_opt;
    }

    return unique_entry_lookup_.lookupOwner(key);
}

auto LookupManager::lookupActivationBlock(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>
{
    std::shared_lock lock{rw_mtx_};

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
                                                daemon_.get());
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
                                                    daemon_.get());
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
                                                             daemon_.get());
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
    -> utilxx::Result<void, ManagerError>
{
    auto block_height = block.getHeight();
    auto block_hash = std::move(block.getHash());

    //traverse all txids to transactions
    auto txs_res =
        traverse(
            std::move(block.getTxids()),
            [this](auto txid) {
                return daemon_
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

    //execute all the operations on the entrys
    processUMEntrys(transactions, block_height);
    processUniqueEntrys(transactions, block_height);
    processUtilityTokens(transactions, block_height);

    //add blockhash to the processed blocks
    block_hashes_.push_back(std::move(block_hash));

    return {};
}

auto LookupManager::lookupIsValid() const
    -> utilxx::Result<bool, daemon::DaemonError>
{
    return getLastValidBlockHeight()
        .map([this](auto last_valid_block) {
            auto starting_block =
                getStartingBlock(daemon_->getCoin());

            return static_cast<std::int64_t>(block_hashes_.size())
                == last_valid_block - starting_block;
        });
}

auto LookupManager::getLastValidBlockHeight() const
    -> utilxx::Result<int64_t, daemon::DaemonError>
{
    auto starting_block = getStartingBlock(daemon_->getCoin());

    std::shared_lock lock{rw_mtx_};
    for(auto&& hash : block_hashes_) {
        if(auto res = daemon_->getBlockHash(++starting_block);
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
    std::shared_lock lock{rw_mtx_};
    return um_entry_lookup_.getUMEntrysOfOwner(owner);
}


auto LookupManager::getUniqueEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::UniqueEntry>
{
    std::shared_lock lock{rw_mtx_};
    return unique_entry_lookup_.getUniqueEntrysOfOwner(owner);
}

auto LookupManager::getUtilityTokensOfOwner(const std::string& owner) const
    -> std::vector<core::UtilityToken>
{
    std::shared_lock lock{rw_mtx_};
    return utility_token_lookup_.getUtilityTokensOfOwner(owner);
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


auto forge::lookup::generateMessage(ManagerError&& error)
    -> std::string
{
    static constexpr auto visitor = utilxx::overload{
        [](LookupError&& error) {
            return fmt::format("LookupError inside ManagerError: {}",
                               std::move(error.what()));
        },
        [](daemon::DaemonError&& error) {
            return fmt::format("DaemonError inside ManagerError: {}",
                               std::move(error.what()));
        }};

    return std::visit(visitor,
                      std::move(error));
}


auto LookupManager::getDaemon() const
    -> const daemon::ReadOnlyDaemonBase&
{
    return *daemon_;
}


auto LookupManager::getCoin() const
    -> core::Coin
{
    return daemon_->getCoin();
}
