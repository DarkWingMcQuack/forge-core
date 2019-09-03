#include "entrys/Entry.hpp"
#include "entrys/uentry/UniqueEntryDeletionOp.hpp"
#include <core/Coin.hpp>
#include <entrys/EntryOperation.hpp>
#include <entrys/umentry/UMEntryCreationOp.hpp>
#include <entrys/umentry/UMEntryRenewalOp.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>
#include <wallet/WalletError.hpp>

using forge::wallet::ReadOnlyWallet;
using forge::wallet::ReadWriteWallet;
using forge::wallet::WalletError;
using forge::core::UMEntry;
using forge::core::UniqueEntry;
using forge::core::getDefaultTxFee;
using forge::core::getMinimumTxAmount;
using forge::core::toHexString;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::createUMEntryCreationOpMetadata;
using forge::core::createUMEntryRenewalOpMetadata;
using forge::core::createUMEntryUpdateOpMetadata;
using forge::core::createUMEntryDeletionOpMetadata;
using utilxx::Result;

ReadWriteWallet::ReadWriteWallet(std::unique_ptr<lookup::LookupManager>&& lookup,
                                 std::unique_ptr<daemon::WriteOnlyDaemonBase>&& daemon)
    : ReadOnlyWallet(std::move(lookup)),
      daemon_(std::move(daemon)) {}


auto ReadWriteWallet::createNewUMEntry(core::EntryKey key,
                                       core::UMEntryValue value,
                                       std::int64_t burn_amount)
    -> Result<std::string, WalletError>
{
    return daemon_
        ->generateNewAddress() //generate new address
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        })
        .onValue([this](auto address) {
            addNewOwnedAddress(std::move(address));
        })
        .flatMap([&](auto address) {
            return createNewUMEntry(std::move(key),
                                    std::move(value),
                                    std::move(address),
                                    burn_amount);
        });
}

auto ReadWriteWallet::createNewUMEntry(core::EntryKey key,
                                       core::UMEntryValue value,
                                       std::string address,
                                       std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create entry
    auto entry = UMEntry{std::move(key),
                         std::move(value)};

    if(!ownesAddress(address)) {
        auto error = fmt::format(
            "it seems that you aren't the owner of "
            "the address {}",
            address);
        return WalletError{std::move(error)};
    }

    //create metadata
    auto metadata =
        createUMEntryCreationOpMetadata(std::move(entry));

    return daemon_
        //send the needed amount to the address
        ->sendToAddress(burn_amount + getDefaultTxFee(getLookup().getCoin()),
                        address)
        .flatMap([&](auto txid) {
            //write the metadata to the blockchain
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                               address)
                .flatMap([&](auto vout_idx) {
                    return daemon_
                        ->burnOutput(std::move(txid),
                                     vout_idx,
                                     std::move(metadata));
                });
        })
        .onValue([&](auto /*unused*/) {
            addNewOwnedAddress(std::move(address));
        })
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        });
}

auto ReadWriteWallet::updateUMEntry(core::EntryKey key,
                                    core::UMEntryValue new_value,
                                    std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //lookup the owner of the key
    auto owner_opt = lookup_->lookupOwner(key);
    if(!owner_opt) {
        auto error =
            fmt::format("unable to lookup the entry key: {}",
                        toHexString(key));
        return WalletError{std::move(error)};
    }

    auto owner = owner_opt.getValue().get();

    if(!ownesAddress(owner)) {
        auto entry_str = toHexString(key);
        auto error = fmt::format(
            "it seems that you aren't the owner of "
            "entry {} which is currently owned by {}",
            entry_str,
            owner);
        return WalletError{std::move(error)};
    }

    //create metadata for the tx
    auto metadata = createUMEntryUpdateOpMetadata(std::move(key),
                                                  std::move(new_value));

    return daemon_
        //move enought funds to the owner address
        ->sendToAddress(burn_amount + getDefaultTxFee(getLookup().getCoin()),
                        std::move(owner))
        .flatMap([&](auto txid) {
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                               owner_opt.getValue().get())
                .flatMap([&](auto vout_idx) {
                    //burn the output with the metadata
                    return daemon_
                        ->burnOutput(std::move(txid),
                                     vout_idx,
                                     std::move(metadata));
                });
        })
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        });
}

auto ReadWriteWallet::createNewUniqueEntry(core::EntryKey key,
                                           core::UMEntryValue value,
                                           std::int64_t burn_amount)
    -> Result<std::string, WalletError>
{
    return daemon_
        ->generateNewAddress() //generate new address
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        })
        .onValue([this](auto address) {
            addNewOwnedAddress(std::move(address));
        })
        .flatMap([&](auto address) {
            return createNewUniqueEntry(std::move(key),
                                        std::move(value),
                                        std::move(address),
                                        burn_amount);
        });
}

auto ReadWriteWallet::createNewUniqueEntry(core::EntryKey key,
                                           core::UMEntryValue value,
                                           std::string address,
                                           std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create entry
    auto entry = UniqueEntry{std::move(key),
                             std::move(value)};

    if(!ownesAddress(address)) {
        auto error = fmt::format(
            "it seems that you aren't the owner of "
            "the address {}",
            address);
        return WalletError{std::move(error)};
    }

    //create metadata
    auto metadata =
        createUniqueEntryCreationOpMetadata(std::move(entry));

    auto default_fee = getDefaultTxFee(getLookup().getCoin());

    return daemon_
        //send the needed amount to the address
        ->sendToAddress(burn_amount + default_fee,
                        address)
        .flatMap([&](auto txid) {
            //write the metadata to the blockchain
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount
                                                   + default_fee,
                                               address)
                .flatMap([&](auto vout_idx) {
                    return daemon_
                        ->burnOutput(std::move(txid),
                                     vout_idx,
                                     std::move(metadata));
                });
        })
        .onValue([&](auto /*unused*/) {
            addNewOwnedAddress(std::move(address));
        })
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        });
}



auto ReadWriteWallet::renewEntry(core::EntryKey key,
                                 std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create an entry and get the owner
    return createEntryOwnerPairFromKey(std::move(key))
        .flatMap([&](auto entry_owner_pair)
                     -> Result<std::pair<std::vector<std::byte>,
                                         std::string>,
                               WalletError> {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);

            if(!ownesAddress(owner)) {
                auto entry_str = core::entryToJson(entry).asString();
                auto error = fmt::format(
                    "it seems that you aren't the owner of "
                    "entry {} which is currently owned by {}",
                    entry_str,
                    owner);
                return WalletError{std::move(error)};
            }

            auto metadata = core::createRenewalOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);


            //send burn amount + fee to the owner address
            return daemon_
                ->sendToAddress(burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                owner)
                .flatMap([&](auto txid) {
                    //burn the output with the metadata
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                                       std::move(owner))
                        .flatMap([&](auto vout_idx) {
                            return daemon_
                                ->burnOutput(std::move(txid),
                                             vout_idx,
                                             std::move(metadata));
                        });
                })
                .mapError([](auto error) {
                    return WalletError{std::move(error.what())};
                });
        });
}

auto ReadWriteWallet::deleteEntry(core::EntryKey key,
                                  std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create an entry and get the owner
    return createEntryOwnerPairFromKey(std::move(key))
        .flatMap([&](auto entry_owner_pair)
                     -> Result<std::pair<std::vector<std::byte>,
                                         std::string>,
                               WalletError> {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);

            if(!ownesAddress(owner)) {
                auto entry_str = core::entryToJson(entry).asString();
                auto error = fmt::format(
                    "it seems that you aren't the owner of "
                    "entry {} which is currently owned by {}",
                    entry_str,
                    owner);
                return WalletError{std::move(error)};
            }

            auto metadata = core::createDeletionOpMetadata(std::move(entry));
            // createUMEntryDeletionOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);

            //send burn amount + fee to the owner address
            return daemon_
                ->sendToAddress(burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                owner)
                .flatMap([&](auto txid) {
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount + getDefaultTxFee(getLookup().getCoin()),
                                                       owner)
                        .flatMap([&](auto vout_idx) {
                            //burn the output with the metadata
                            return daemon_
                                ->burnOutput(std::move(txid),
                                             vout_idx,
                                             std::move(metadata));
                        });
                })
                .mapError([](auto&& error) {
                    return WalletError{std::move(error.what())};
                });
        });
}

auto ReadWriteWallet::transferOwnership(core::EntryKey key,
                                        std::string new_owner,
                                        std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    return createEntryOwnerPairFromKey(std::move(key))
        .flatMap([&](auto entry_owner_pair)
                     -> Result<std::pair<std::vector<std::byte>,
                                         std::string>,
                               WalletError> {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);

            if(!ownesAddress(owner)) {
                auto entry_str = core::entryToJson(entry).asString();
                auto error = fmt::format(
                    "it seems that you aren't the owner of "
                    "entry {} which is currently owned by {}",
                    entry_str,
                    owner);
                return WalletError{std::move(error)};
            }

            auto metadata = createOwnershipTransferOpMetadata(std::move(entry));
            // createUMEntryOwnershipTransferOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            auto coin = getLookup().getCoin();

            //send burn amount + fee to the owner address
            return daemon_
                //send +minTxAmount because this will be send to the new owner address
                ->sendToAddress(burn_amount
                                    + getMinimumTxAmount(coin)
                                    + getDefaultTxFee(getLookup().getCoin()),
                                owner)
                .flatMap([&](auto txid) {
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount
                                                           + getMinimumTxAmount(coin)
                                                           + getDefaultTxFee(getLookup().getCoin()),
                                                       std::move(owner))
                        .flatMap([&](auto vout_idx) {
                            return daemon_
                                ->burnAmount(std::move(txid),
                                             vout_idx,
                                             burn_amount,
                                             std::move(metadata),
                                             std::move(new_owner));
                        });
                })
                .mapError([](auto error) {
                    return WalletError{std::move(error.what())};
                });
        });
}

auto ReadWriteWallet::payToEntryOwner(core::EntryKey key,
                                      std::int64_t amount)
    -> utilxx::Result<std::string, WalletError>
{
    //lookup the owner of the key
    auto owner_opt = lookup_->lookupOwner(key);
    if(!owner_opt) {
        auto error =
            fmt::format("unable to lookup the entry key: {}",
                        toHexString(key));
        return WalletError{std::move(error)};
    }

    auto owner = owner_opt.getValue().get();

    return daemon_
        ->sendToAddress(amount,
                        std::move(owner))
        .mapError([](auto error) {
            return WalletError{std::move(error.what())};
        });
}

auto ReadWriteWallet::createEntryOwnerPairFromKey(core::EntryKey key)
    -> utilxx::Result<std::pair<core::Entry,
                                std::string>,
                      WalletError>
{
    auto entry_opt = lookup_->lookup(key);
    auto owner_opt = lookup_->lookupOwner(key);
    auto lookup_opt = utilxx::combine(std::move(entry_opt),
                                      std::move(owner_opt));
    if(!lookup_opt) {
        auto error =
            fmt::format("unable to lookup the entry key: {}",
                        toHexString(key));
        return WalletError{std::move(error)};
    }

    auto entry = lookup_opt.getValue().first;
    auto owner = lookup_opt.getValue().second.get();

    return std::pair{std::move(entry),
                     std::move(owner)};
}
