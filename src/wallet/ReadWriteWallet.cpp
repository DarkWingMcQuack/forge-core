#include <core/Coin.hpp>
#include <core/EntryCreationOp.hpp>
#include <core/EntryRenewalOp.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>
#include <wallet/WalletError.hpp>

using buddy::wallet::ReadOnlyWallet;
using buddy::wallet::ReadWriteWallet;
using buddy::wallet::WalletError;
using buddy::core::Entry;
using buddy::core::getDefaultTxFee;
using buddy::core::toHexString;
using buddy::core::EntryKey;
using buddy::core::EntryValue;
using buddy::core::createEntryCreationOpMetadata;
using buddy::core::createEntryRenewalOpMetadata;
using buddy::core::createEntryUpdateOpMetadata;
using buddy::core::createEntryDeletionOpMetadata;
using utilxx::Result;

ReadWriteWallet::ReadWriteWallet(std::unique_ptr<lookup::LookupManager>&& lookup,
                                 std::unique_ptr<daemon::WriteOnlyDaemonBase>&& daemon,
                                 core::Coin coin)
    : ReadOnlyWallet(std::move(lookup)),
      daemon_(std::move(daemon)),
      coin_(coin) {}


auto ReadWriteWallet::createNewEntry(core::EntryKey&& key,
                                     core::EntryValue&& value,
                                     std::int64_t burn_amount)
    -> Result<std::string, WalletError>
{
    return daemon_
        ->generateNewAddress() //generate new address
        .mapError([](auto&& error) {
            return WalletError{std::move(error.what())};
        })
        .flatMap([&](auto&& address) {
            return createNewEntry(std::move(key),
                                  std::move(value),
                                  std::move(address),
                                  burn_amount);
        });
}

auto ReadWriteWallet::createNewEntry(core::EntryKey&& key,
                                     core::EntryValue&& value,
                                     std::string&& address,
                                     std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create entry
    auto entry = Entry{std::move(key),
                       std::move(value)};

    //create metadata
    auto metadata =
        createEntryCreationOpMetadata(std::move(entry));

    return daemon_
        //send the needed amount to the address
        ->sendToAddress(burn_amount + getDefaultTxFee(coin_),
                        std::move(address))
        .flatMap([&](auto&& txid) {
            //write the metadata to the blockchain
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount + getDefaultTxFee(coin_),
                                               address)
                .flatMap([&](auto vout_idx) {
                    return daemon_
                        ->burnOutput(std::move(txid),
                                     vout_idx,
                                     std::move(metadata));
                });
        })
        .mapError([](auto&& error) {
            return WalletError{std::move(error.what())};
        });
}


auto ReadWriteWallet::renewEntry(core::EntryKey&& key,
                                 std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create an entry and get the owner
    return createEntryOwnerPairFromKey(std::move(key))
        .map([&](auto&& entry_owner_pair) {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);
            auto metadata =
                createEntryRenewalOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto&& pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            auto owner_copy = owner;

            //send burn amount + fee to the owner address
            return daemon_
                ->sendToAddress(burn_amount + getDefaultTxFee(coin_),
                                std::move(owner_copy))
                .flatMap([&](auto&& txid) {
                    //burn the output with the metadata
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount + getDefaultTxFee(coin_),
                                                       owner)
                        .flatMap([&](auto vout_idx) {
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

auto ReadWriteWallet::updateEntry(core::EntryKey&& key,
                                  core::EntryValue&& new_value,
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

    //create metadata for the tx
    auto metadata = createEntryUpdateOpMetadata(std::move(key),
                                                std::move(new_value));

    return daemon_
        //move enought funds to the owner address
        ->sendToAddress(burn_amount + getDefaultTxFee(coin_),
                        std::move(owner))
        .flatMap([&](auto&& txid) {
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount + getDefaultTxFee(coin_),
                                               owner_opt.getValue().get())
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
}

auto ReadWriteWallet::deleteEntry(core::EntryKey&& key,
                                  std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create an entry and get the owner
    return createEntryOwnerPairFromKey(std::move(key))
        .map([&](auto&& entry_owner_pair) {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);
            auto metadata =
                createEntryDeletionOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto&& pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            auto owner_copy = owner;

            //send burn amount + fee to the owner address
            return daemon_
                ->sendToAddress(burn_amount + getDefaultTxFee(coin_),
                                std::move(owner_copy))
                .flatMap([&](auto&& txid) {
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount + getDefaultTxFee(coin_),
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

auto ReadWriteWallet::transferOwnership(core::EntryKey&& key,
                                        std::string&& new_owner,
                                        std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    return createEntryOwnerPairFromKey(std::move(key))
        .map([&](auto&& entry_owner_pair) {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);
            auto metadata =
                createOwnershipTransferOpMetadata(std::move(entry));

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto&& pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            auto owner_copy = owner;

            //send burn amount + fee to the owner address
            return daemon_
                //send +1 because this will be send to the new owner address
                ->sendToAddress(burn_amount + 1 + getDefaultTxFee(coin_),
                                std::move(owner_copy))
                .flatMap([&](auto&& txid) {
                    return daemon_
                        ->getVOutIdxByAmountAndAddress(txid,
                                                       burn_amount + getDefaultTxFee(coin_),
                                                       owner)
                        .flatMap([&](auto vout_idx) {
                            return daemon_
                                ->burnAmount(std::move(txid),
                                             vout_idx,
                                             burn_amount,
                                             std::move(metadata),
                                             std::move(new_owner));
                        });
                })
                .mapError([](auto&& error) {
                    return WalletError{std::move(error.what())};
                });
        });
}

auto ReadWriteWallet::payToEntryOwner(core::EntryKey&& key,
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
        .mapError([](auto&& error) {
            return WalletError{std::move(error.what())};
        });
}

auto ReadWriteWallet::createEntryOwnerPairFromKey(core::EntryKey&& key)
    -> utilxx::Result<std::pair<core::Entry,
                                std::string>,
                      WalletError>
{
    auto value_opt = lookup_->lookupValue(key);
    auto owner_opt = lookup_->lookupOwner(key);
    auto lookup_opt = utilxx::combine(std::move(value_opt),
                                      std::move(owner_opt));
    if(!lookup_opt) {
        auto error =
            fmt::format("unable to lookup the entry key: {}",
                        toHexString(key));
        return WalletError{std::move(error)};
    }

    auto value = lookup_opt.getValue().first.get();

    auto entry = Entry{std::move(key),
                       std::move(value)};

    auto owner = lookup_opt.getValue().second.get();

    return std::pair{std::move(entry),
                     std::move(owner)};
}
