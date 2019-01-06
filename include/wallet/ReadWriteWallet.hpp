#pragma once

#include <core/Entry.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/WalletError.hpp>

namespace buddy::wallet {

class ReadWriteWallet : public ReadOnlyWallet
{
public:
    ReadWriteWallet(std::unique_ptr<lookup::LookupManager>&& lookup,
                    std::unique_ptr<daemon::WriteOnlyDaemonBase>&& daemon,
                    core::Coin coin);
    //creates a new entry key value pair on the blockchain
    //using any output of the dameonwallet
    //coins will be send to a new address and this output
    //will be used to create the OP_RETURN transaction
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewEntry(core::EntryKey&& key,
                        core::EntryValue&& value,
                        std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //creates a new entry key value pair on the blockchain
    //using the given address as owner of the entry
    //funds will be trnasfered to this address to be able to
    //do the burn with the given value
    //on success, the txid of the burn is returned
    //on error a WalletError is returned
    auto createNewEntry(core::EntryKey&& key,
                        core::EntryValue&& value,
                        std::string&& address,
                        std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //renews the lifetime of a given key-value pair on the blockchain
    //the burn_value will be burned in the OP_RETURN tx
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if the the wallet actualy is the owner of the entry
    //and the daemon wallet has enough funds to do the renewal with the
    //given parameters the renewal is valid and will be executed
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto renewEntry(core::EntryKey&& key,
                    std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;


    //updates the value of a given entry-key
    //to perform this operation the wallet needs to be the owner of the
    //entry on the blockchain and needs to have more than the burn_amount
    //in coins
    //coins will be transfered to the owner address which then will be used
    //for the OP_RETURN transaction
    //if so the operation is executed and a updateing TX is created
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto updateEntry(core::EntryKey&& key,
                     core::EntryValue&& new_value,
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
    auto deleteEntry(core::EntryKey&& key,
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
    auto transferOwnership(core::EntryKey&& key,
                           std::string&& new_owner,
                           std::int64_t burn_amount)
        -> utilxx::Result<std::string, WalletError>;

    //looks up the owner of a given entry
    //then transfers the given number of coins to the
    //owner of the entry
    //can be used as a DNS for payments to usernames
    //on success the txid of the transaction renewing the enty will be returned
    //on error a WalletError holding the reason will be returned
    auto payToEntryOwner(core::EntryKey&& key,
                         std::int64_t amount)
        -> utilxx::Result<std::string, WalletError>;

private:
    auto createEntryOwnerPairFromKey(core::EntryKey&& key)
        -> utilxx::Result<std::pair<core::Entry,
                                    std::string>,
                          WalletError>;

private:
    std::unique_ptr<daemon::WriteOnlyDaemonBase> daemon_;
    core::Coin coin_;
};

} // namespace buddy::wallet
