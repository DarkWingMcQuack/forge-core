#include <core/Operation.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <functional>
#include <lookup/EntryLookup.hpp>
#include <lookup/LookupManager.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::lookup::LookupManager;
using buddy::lookup::EntryLookup;
using buddy::lookup::LookupError;
using buddy::core::EntryKey;
using buddy::core::EntryValue;
using buddy::util::Opt;
using buddy::util::Result;
using buddy::daemon::DaemonBase;
using buddy::daemon::getMaturity;

LookupManager::LookupManager(std::unique_ptr<daemon::DaemonBase> daemon)
    : daemon_(std::move(daemon)),
      lookup_(),
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
        .flatMap([&](auto actual_height) mutable
                 -> Result<void, ManagerError> {
            //if we have less blocks than maturity
            //then nothing happens
            if(actual_height < maturity) {
                return {};
            }

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
                    return res;
                }
                //update blockheight of lookup
                lookup_.setBlockHeight(current_height);
            }

            return {};
        });
}
