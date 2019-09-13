#pragma once

#include <core/Coin.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <entrys/Entry.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/WalletError.hpp>

namespace forge::wallet {

class ReadWriteWallet : public ReadOnlyWallet
{
public:
    ReadWriteWallet(std::unique_ptr<lookup::LookupManager>&& lookup,
                    std::unique_ptr<daemon::WriteOnlyDaemonBase>&& daemon);
    //creates a new entry key value pair on the blockchain
    //using any output of the dameonwallet
    //coins will be send to a new address and this output
    //will be used to create the OP_RETURN transaction
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUMEntry(core::EntryKey key,
                          core::UMEntryValue value,
                          std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //creates a new entry key value pair on the blockchain
    //using the given address as owner of the entry
    //funds will be trnasfered to this address to be able to
    //do the burn with the given value
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUMEntry(core::EntryKey key,
                          core::UMEntryValue value,
                          std::string address,
                          std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //updates the value of a given umentry
    //to perform this operation the wallet needs to be the owner of the
    //entry on the blockchain and needs to have more than the burn_amount
    //in coins
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if so the operation is executed and a updateing TX is created
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto updateUMEntry(core::EntryKey key,
                       core::UMEntryValue new_value,
                       std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;


    //creates a new non modifiable unique entry on the blockchain
    //using any output of the dameonwallet
    //coins will be send to a new address and this output
    //will be used to create the OP_RETURN transaction
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUniqueEntry(core::EntryKey key,
                              core::UniqueEntryValue value,
                              std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //creates a new non modifiable unique entry on the blockchain
    //using the given address as owner of the entry
    //funds will be trnasfered to this address to be able to
    //do the burn with the given value
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUniqueEntry(core::EntryKey key,
                              core::UniqueEntryValue value,
                              std::string address,
                              std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //creates a new utility token on the blockchain
    //using any output of the dameonwallet
    //coins will be send to a new address and this output
    //will be used to create the OP_RETURN transaction
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUtilityToken(core::EntryKey id,
                               std::uint64_t supply,
                               std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //creates a new utility token on the blockchain
    //using the given address as owner of the entry
    //funds will be trnasfered to this address to be able to
    //do the burn with the given value
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewUtilityToken(core::EntryKey id,
                               std::uint64_t supply,
                               std::string address,
                               std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //renews the lifetime of a given entry pair on the blockchain
    //the burn_value will be burned in the OP_RETURN tx
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if the the wallet actualy is the owner of the entry
    //and the daemon wallet has enough funds to do the renewal with the
    //given parameters the renewal is valid and will be executed
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto renewEntry(core::EntryKey key,
                    std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //deletes an Entry
    //to perform this operation the wallet needs to be the owner of the
    //entry on the blockchain and needs to have more than the burn_amount
    //in coins
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if so the operation is executed and a updateing TX is created
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto deleteEntry(core::EntryKey key,
                     std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //transferes the ownership of a given entry to another address
    //to perform this operation the wallet needs to be the owner of the
    //entry on the blockchain and needs to have more than the burn_amount
    //in coins
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if so the operation is executed and a updateing TX is created
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto transferOwnership(core::EntryKey key,
                           std::string new_owner,
                           std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //transfers a given *amount* of utility tokens to a *new_owner*
    auto transferUtilityTokens(core::EntryKey id,
                               std::string new_owner,
                               std::uint64_t amount,
                               std::int64_t burn_amount)
        -> utilxx::Result<std::vector<std::string>,
                          WalletError>;

    //delete/burns a given *amount* of utility tokens if they
    //are owned by the wallet
    auto deleteUtilityTokens(core::EntryKey id,
                             std::uint64_t amount,
                             std::int64_t burn_amount)
        -> utilxx::Result<std::vector<std::string>,
                          WalletError>;

    //looks up the owner of a given entry
    //then transfers the given number of coins to the
    //owner of the entry
    //can be used as a DNS for payments to usernames
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto payToEntryOwner(core::EntryKey key,
                         std::int64_t amount)
        -> utilxx::Result<std::string, WalletError>;

private:
    auto createEntryOwnerPairFromKey(core::EntryKey key)
        -> utilxx::Result<std::pair<core::Entry,
                                    std::string>,
                          WalletError>;

    auto createRenewableEntryOwnerPairFromKey(core::EntryKey key)
        -> utilxx::Result<std::pair<core::RenewableEntry,
                                    std::string>,
                          WalletError>;
    //returns a vector of pairs (string, int) where the string is the address
    //and the int is the amount of the token to be send from the address to
    //be able to send desired_amount in total
    auto getUtilityTokenSendVector(const std::vector<std::byte>& token,
                                   std::uint64_t desired_amount)
        -> utilxx::Result<
            std::vector<
                std::pair<std::string,
                          std::uint64_t>>,
            WalletError>;

    auto burn(const std::string& address,
              std::int64_t burn_amount,
              std::vector<std::byte> metadata)
        -> utilxx::Result<std::string, WalletError>;

    auto burn(const std::string& owner,
              const std::string& new_owner,
              std::int64_t burn_amount,
              std::vector<std::byte> metadata)
        -> utilxx::Result<std::string, WalletError>;

private:
    std::unique_ptr<daemon::WriteOnlyDaemonBase> daemon_;
};

} // namespace forge::wallet
