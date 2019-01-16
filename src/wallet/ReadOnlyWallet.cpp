#include <lookup/LookupManager.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>

using buddy::lookup::LookupManager;
using buddy::lookup::LookupError;
using buddy::wallet::ReadOnlyWallet;
using buddy::core::Entry;


ReadOnlyWallet::ReadOnlyWallet(std::unique_ptr<lookup::LookupManager>&& lookup)
    : lookup_(std::move(lookup))
{
    lookup_
        ->getDaemon()
        .getAddresses()
        .onValue([this](auto&& addresses) {
            for(auto&& addr : addresses) {
                this->addNewOwnedAddress(std::move(addr));
            }
        });
}

auto ReadOnlyWallet::addWatchOnlyAddress(std::string&& adr)
    -> void
{
    watched_addresses_.insert(std::move(adr));
}

auto ReadOnlyWallet::deleteWatchOnlyAddress(const std::string& adr)
    -> void
{
    watched_addresses_.erase(adr);
}

auto ReadOnlyWallet::addNewOwnedAddress(std::string&& adr)
    -> void
{
    owned_addresses_.insert(std::move(adr));
}

auto ReadOnlyWallet::getOwnedEntrys() const
    -> std::vector<Entry>
{
    std::vector<Entry> ret_vec;
    for(auto&& addr : owned_addresses_) {
        auto new_entrys = lookup_->getEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getWatchOnlyEntrys() const
    -> std::vector<Entry>
{
    std::vector<Entry> ret_vec;
    for(auto&& addr : watched_addresses_) {
        auto new_entrys = lookup_->getEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getAllWatchedEntrys() const
    -> std::vector<Entry>
{
    auto owned_entrys = getOwnedEntrys();
    auto watched_entrys = getWatchOnlyEntrys();

    owned_entrys.insert(std::begin(owned_entrys),
                        std::make_move_iterator(std::begin(watched_entrys)),
                        std::make_move_iterator(std::end(watched_entrys)));

    return owned_entrys;
}

auto ReadOnlyWallet::getWatchedAddresses() const
    -> const std::set<std::string>&
{
    return watched_addresses_;
}

auto ReadOnlyWallet::getOwnedAddresses() const
    -> const std::set<std::string>&
{
    return owned_addresses_;
}

auto ReadOnlyWallet::getLookup() const
    -> const lookup::LookupManager&
{
    return *lookup_;
}

auto ReadOnlyWallet::getLookup()
    -> lookup::LookupManager&
{
    return *lookup_;
}
