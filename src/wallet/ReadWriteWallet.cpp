#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <entrys/Entry.hpp>
#include <entrys/EntryOperation.hpp>
#include <entrys/token/UtilityTokenOwnershipTransferOp.hpp>
#include <entrys/uentry/UniqueEntryDeletionOp.hpp>
#include <entrys/umentry/UMEntryCreationOp.hpp>
#include <entrys/umentry/UMEntryRenewalOp.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <utilxx/Opt.hpp>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>
#include <wallet/WalletError.hpp>

using forge::wallet::ReadOnlyWallet;
using forge::wallet::ReadWriteWallet;
using forge::wallet::WalletError;
using forge::core::UMEntry;
using forge::core::UtilityToken;
using forge::core::UniqueEntry;
using forge::core::getDefaultTxFee;
using forge::core::getMinimumTxAmount;
using forge::core::toHexString;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::createUMEntryCreationOpMetadata;
using forge::core::createUMEntryUpdateOpMetadata;
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

    return burn(address,
                burn_amount,
                metadata);
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

    return burn(owner,
                burn_amount,
                metadata);
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


auto ReadWriteWallet::createNewUtilityToken(core::EntryKey id,
                                            std::uint64_t supply,
                                            std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
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
            return createNewUtilityToken(std::move(id),
                                         supply,
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

    return burn(address,
                burn_amount,
                metadata);
}


auto ReadWriteWallet::createNewUtilityToken(core::EntryKey id,
                                            std::uint64_t supply,
                                            std::string address,
                                            std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create entry
    auto entry = UtilityToken{std::move(id),
                              std::move(supply)};

    if(!ownesAddress(address)) {
        auto error = fmt::format(
            "it seems that you aren't the owner of "
            "the address {}",
            address);
        return WalletError{std::move(error)};
    }

    //create metadata
    auto metadata =
        createUtilityTokenCreationOpMetadata(std::move(entry));

    return burn(address,
                burn_amount,
                metadata);
}

auto ReadWriteWallet::renewEntry(core::EntryKey key,
                                 std::int64_t burn_amount)
    -> utilxx::Result<std::string, WalletError>
{
    //create an entry and get the owner
    return createRenewableEntryOwnerPairFromKey(std::move(key))
        .flatMap([&](auto entry_owner_pair)
                     -> Result<std::pair<std::vector<std::byte>,
                                         std::string>,
                               WalletError> {
            //create metadata for the OP_RETURN burn
            auto [entry, owner] = std::move(entry_owner_pair);

            if(!ownesAddress(owner)) {
                auto entry_str = std::visit(
                    [](const auto& en) {
                        return core::entryToJson(en).asString();
                    },
                    entry);
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
            return burn(owner,
                        burn_amount,
                        metadata);
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

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            return burn(owner,
                        burn_amount,
                        metadata);
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

            //return metadata and the owner
            return std::pair{std::move(metadata),
                             std::move(owner)};
        })
        .flatMap([&](auto pair) {
            //extract metadata and owner
            auto [metadata, owner] = std::move(pair);
            return burn(owner,
                        new_owner,
                        burn_amount,
                        metadata);
        });
}

auto ReadWriteWallet::transferUtilityTokens(core::EntryKey id,
                                            std::string new_owner,
                                            std::uint64_t amount,
                                            std::int64_t burn_amount)
    -> utilxx::Result<std::vector<std::string>,
                      WalletError>
{
    auto send_list_res = getUtilityTokenSendVector(id,
                                                   amount);

    if(!send_list_res) {
        return send_list_res.getError();
    }
    auto send_list = std::move(send_list_res.getValue());

    return utilxx::traverse(
        std::move(send_list),
        [&](auto triple) {
            auto [address, used] = std::move(triple);
            UtilityToken token{id, used};
            auto metadata =
                createUtilityTokenOwnershipTransferOpMetadata(std::move(token));
            return burn(address,
                        new_owner,
                        burn_amount,
                        std::move(metadata));
        });
}

auto ReadWriteWallet::deleteUtilityTokens(core::EntryKey id,
                                          std::uint64_t amount,
                                          std::int64_t burn_amount)
    -> utilxx::Result<std::vector<std::string>,
                      WalletError>
{
    auto send_list_res = getUtilityTokenSendVector(id,
                                                   amount);

    if(!send_list_res) {
        return send_list_res.getError();
    }
    auto send_list = std::move(send_list_res.getValue());

    return utilxx::traverse(
        std::move(send_list),
        [&](auto triple) {
            auto [address, used] = std::move(triple);
            UtilityToken token{id, used};
            auto metadata =
                createUtilityTokenDeletionOpMetadata(std::move(token));
            return burn(address,
                        burn_amount,
                        std::move(metadata));
        });
}

auto ReadWriteWallet::getUtilityTokenSendVector(const std::vector<std::byte>& token,
                                                std::uint64_t desired_amount)
    -> utilxx::Result<
        std::vector<
            std::pair<std::string,
                      std::uint64_t>>,
        WalletError>
{
    std::vector<std::pair<std::string, //address
                          std::uint64_t>> //used for this transaction
        owned_token_balances;

    std::uint64_t covered{0};
    for(auto&& address : owned_addresses_) {
        auto balance = lookup_->getUtilityTokenCreditOf(address,
                                                        token);

        if(balance != 0) {
            auto used = std::min(balance, desired_amount - covered);
            owned_token_balances.emplace_back(address,
                                              used);
        }

        if(covered == desired_amount) {
            break;
        }
    }

    if(covered != desired_amount) {
        auto token_str = toHexString(token);
        auto error =
            fmt::format("insufficient funds of token {}, needed: {}, available: {}",
                        token_str,
                        desired_amount,
                        covered);
        return WalletError{std::move(error)};
    }

    return owned_token_balances;
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

auto ReadWriteWallet::createRenewableEntryOwnerPairFromKey(core::EntryKey key)
    -> utilxx::Result<std::pair<core::RenewableEntry,
                                std::string>,
                      WalletError>
{
    auto entry_opt = [&]()
        -> utilxx::Opt<core::RenewableEntry> {
        if(auto um_entry_opt = lookup_->lookupUMValue(key);
           um_entry_opt) {
            return core::RenewableEntry{
                UMEntry(std::move(key),
                        um_entry_opt.getValue().get())};
        }

        if(auto unique_entry_opt = lookup_->lookupUniqueValue(key);
           unique_entry_opt) {
            return core::RenewableEntry{
                UniqueEntry(std::move(key),
                            unique_entry_opt.getValue().get())};
        }

        return std::nullopt;
    }();

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

auto ReadWriteWallet::burn(const std::string& address,
                           std::int64_t burn_amount,
                           std::vector<std::byte> metadata)
    -> utilxx::Result<std::string, WalletError>
{
    auto default_fee = getDefaultTxFee(getLookup().getCoin());

    return daemon_
        //send the needed amount to the address
        ->sendToAddress(burn_amount
                            + default_fee,
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

auto ReadWriteWallet::burn(const std::string& owner,
                           const std::string& new_owner,
                           std::int64_t burn_amount,
                           std::vector<std::byte> metadata)
    -> utilxx::Result<std::string, WalletError>
{
    auto coin = getLookup().getCoin();

    return daemon_
        //send +minTxAmount because this will be send to the new owner address
        ->sendToAddress(burn_amount
                            + getMinimumTxAmount(coin)
                            + getDefaultTxFee(coin),
                        owner)
        .flatMap([&](auto txid) {
            return daemon_
                ->getVOutIdxByAmountAndAddress(txid,
                                               burn_amount
                                                   + getMinimumTxAmount(coin)
                                                   + getDefaultTxFee(coin),
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
}
