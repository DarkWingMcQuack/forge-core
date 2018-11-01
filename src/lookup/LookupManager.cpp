#include <core/Operation.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <fmt/core.h>
#include <functional>
#include <g3log/g3log.hpp>
#include <lookup/EntryLookup.hpp>
#include <lookup/LookupManager.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::lookup::LookupManager;
using buddy::lookup::EntryLookup;
using buddy::lookup::LookupError;
using buddy::core::EntryKey;
using buddy::core::EntryValue;
using buddy::core::Operation;
using buddy::util::Opt;
using buddy::util::Result;
using buddy::util::traverse;
using buddy::daemon::DaemonBase;
using buddy::daemon::getMaturity;

LookupManager::LookupManager(std::unique_ptr<daemon::DaemonBase> daemon)
    : daemon_(std::move(daemon)),
      lookup_(getStartingBlock(daemon_->getCoin())),
      block_hashes_() {}

auto LookupManager::updateLookup()
    -> util::Result<void, ManagerError>
{
    const auto maturity = getMaturity(daemon_->getCoin());
    auto current_height = lookup_.getBlockHeight();

    return daemon_->getBlockCount()
        .mapError([](auto&& error) {
            return ManagerError{std::move(error)};
        })
        .flatMap([&](auto actual_height)
                     -> Result<void, ManagerError> {
            //if we have less blocks than maturity
            //then nothing happens
            if(actual_height < maturity) {
                LOG(DEBUG) << "not enough blocks to have any mature blocks";
                return {};
            }

            //process missing blocks
            while(actual_height - maturity > current_height) {
                LOG(DEBUG) << "node hight: " << actual_height
                           << " buddy height: " << current_height
                           << " coin maturity: " << maturity;
                auto res =
                    //get block hash
                    daemon_->getBlockHash(++current_height)
                        .flatMap([&](auto&& block_hash) {
                            //get block
                            LOG(DEBUG) << "get block " << block_hash;
                            return daemon_->getBlock(std::move(block_hash));
                        })
                        .mapError([](auto&& error) {
                            return ManagerError{std::move(error)};
                        })
                        //process the block
                        .flatMap([&](auto&& block) {
                            LOG(DEBUG) << "processing Block: " << current_height;
                            return processBlock(std::move(block));
                        });

                //if an error occured return the error
                if(!res) {
                    return res;
                }

                //update blockheight of lookup
                lookup_.setBlockHeight(current_height);
            }

            return {};
        });
}

auto LookupManager::rebuildLookup()
    -> util::Result<void, ManagerError>
{
    lookup_.clear();
    updateLookup();
}

auto LookupManager::lookupValue(const core::EntryKey& key) const
    -> util::Opt<std::reference_wrapper<const core::EntryValue>>
{
    return lookup_.lookup(key);
}

auto LookupManager::lookupOwner(const core::EntryKey& key) const
    -> util::Opt<std::reference_wrapper<const std::string>>
{
    return lookup_.lookupOwner(key);
}

auto LookupManager::processBlock(core::Block&& block)
    -> util::Result<void, ManagerError>
{
    auto block_height = block.getHeight();
    auto block_hash = std::move(block.getHash());

    //traverse all txids to transactions
    return traverse(std::move(block.getTxids()),
                    [this](auto&& txid) {
                        LOG(DEBUG) << "get transaction: " << txid;

                        return daemon_
                            ->getTransaction(std::move(txid))
                            .mapError([](auto&& error) {
                                return ManagerError{std::move(error)};
                            });
                    })
        .flatMap([&](auto&& txs)
                     -> Result<void, ManagerError> {
            //vector of all buddy ops in the block
            std::vector<Operation> ops;
            //exract all buddy ops from the txs
            for(auto&& tx : txs) {
                std::string_view txid = tx.getTxid();

                auto op_res = parseTransactionToEntry(std::move(tx),
                                                      block_height,
                                                      daemon_);
                //if we dont get an opt, but an error, we return it
                if(!op_res) {
                    return ManagerError{std::move(op_res.getError())};
                }

                LOG_IF(DEBUG, !op_res.getValue().hasValue()) << "throw away " << txid;
                LOG_IF(DEBUG, op_res.getValue().hasValue()) << "found operation " << txid;

                //check if the operation was parsed, and if
                //we add it to the std::vector<Operation> vec
                if(auto op_opt = std::move(op_res.getValue());
                   op_opt) {
                    ops.push_back(std::move(op_opt.getValue()));
                }
            }

            //execture the operations by the lookup

            LOG(DEBUG) << "execute " << ops.size()
                       << " operations from block " << block_height;
            return lookup_
                .executeOperations(std::move(ops))
                .mapError([](auto&& error) {
                    return ManagerError{std::move(error)};
                });
        })
        .onValue([&block_hash,
                  this] {
            LOG(DEBUG) << "add blockhash " << block_hash;
            block_hashes_.push_back(std::move(block_hash));
        });
}

auto LookupManager::lookupIsValid() const
    -> util::Result<bool, daemon::DaemonError>
{
    auto starting_block = getStartingBlock(daemon_->getCoin());

    for(auto&& hash : block_hashes_) {
        if(auto res = daemon_->getBlockHash(starting_block++);
           res) {
            if(res.getValue() != hash) {
                return false;
            }
        } else {
            return res.getError();
        }
    }

    return true;
}
