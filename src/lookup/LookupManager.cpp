#include <core/Coin.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <fmt/core.h>
#include <functional>
#include <g3log/g3log.hpp>
#include <lookup/LookupManager.hpp>
#include <lookup/UMEntryLookup.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <utilxx/Result.hpp>

using forge::lookup::LookupManager;
using forge::lookup::UMEntryLookup;
using forge::lookup::LookupError;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::UMEntryOperation;
using forge::core::getMaturity;
using utilxx::Opt;
using utilxx::Result;
using utilxx::traverse;
using forge::daemon::ReadOnlyDaemonBase;

LookupManager::LookupManager(std::unique_ptr<daemon::ReadOnlyDaemonBase>&& daemon)
    : daemon_(std::move(daemon)),
      lookup_(getStartingBlock(daemon_->getCoin())),
      block_hashes_() {}

auto LookupManager::updateLookup()
    -> utilxx::Result<bool, ManagerError>
{
    //aquire writer lock
    std::unique_lock lock{rw_mtx_};
    const auto maturity = getMaturity(daemon_->getCoin());
    auto current_height = lookup_.getBlockHeight();

    return daemon_->getBlockCount()
        .mapError([](auto&& error) {
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
            while(actual_height - maturity > current_height) {
                auto res =
                    //get block hash
                    daemon_->getBlockHash(++current_height)
                        .flatMap([&](auto&& block_hash) {
                            //get block
                            return daemon_->getBlock(std::move(block_hash));
                        })
                        .mapError([](auto&& error) {
                            return ManagerError{std::move(error)};
                        })
                        //process the block
                        .flatMap([&](auto&& block) {
                            return processBlock(std::move(block));
                        });

                //if an error occured return the error
                if(!res) {
                    return res.getError();
                }

                new_block_added = true;

                //update blockheight of lookup
                lookup_.setBlockHeight(current_height);
            }

            return new_block_added;
        });
}

auto LookupManager::rebuildLookup()
    -> utilxx::Result<void, ManagerError>
{
    std::unique_lock lock{rw_mtx_};
    lookup_.clear();
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
    return lookup_.lookup(key);
}

auto LookupManager::lookupOwner(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const std::string>>
{
    std::shared_lock lock{rw_mtx_};
    return lookup_.lookupOwner(key);
}

auto LookupManager::lookupActivationBlock(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>
{
    std::shared_lock lock{rw_mtx_};
    return lookup_.lookupActivationBlock(key);
}

auto LookupManager::processBlock(core::Block&& block)
    -> utilxx::Result<void, ManagerError>
{
    auto block_height = block.getHeight();
    auto block_hash = std::move(block.getHash());

    //traverse all txids to transactions
    return traverse(std::move(block.getTxids()),
                    [this](auto&& txid) {
                        return daemon_
                            ->getTransaction(std::move(txid))
                            .mapError([](auto&& error) {
                                return ManagerError{std::move(error)};
                            });
                    })
        .flatMap([&](auto&& txs)
                     -> Result<void, ManagerError> {
            //vector of all forge ops in the block
            std::vector<UMEntryOperation> ops;
            //exract all forge ops from the txs
            for(auto&& tx : txs) {
                std::string_view txid = tx.getTxid();

                auto op_res = parseTransactionToUMEntry(std::move(tx),
                                                        block_height,
                                                        daemon_.get());
                //if we dont get an opt, but an error, we return it
                if(!op_res) {
                    LOG(WARNING) << op_res.getError().what();
                    continue;
                }

                LOG_IF(DEBUG, op_res.getValue().hasValue()) << "found operation " << txid;

                //check if the operation was parsed, and if
                //we add it to the std::vector<UMEntryOperation> vec
                if(auto op_opt = std::move(op_res.getValue());
                   op_opt) {
                    ops.push_back(std::move(op_opt.getValue()));
                }
            }

            //execture the operations by the lookup

            LOG_IF(DEBUG, !ops.empty())
                << "execute "
                << ops.size()
                << " operation(s) from block " << block_hash;

            return lookup_
                .executeOperations(std::move(ops))
                .mapError([](auto&& error) {
                    return ManagerError{std::move(error)};
                });
        })
        .onValue([&block_hash,
                  this] {
            block_hashes_.push_back(std::move(block_hash));
        });
}

auto LookupManager::lookupIsValid() const
    -> utilxx::Result<bool, daemon::DaemonError>
{
    return getLastValidBlockHeight()
        .map([this](auto&& last_valid_block) {
            auto starting_block =
                getStartingBlock(daemon_->getCoin());

            return block_hashes_.size()
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
    return lookup_.getUMEntrysOfOwner(owner);
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
