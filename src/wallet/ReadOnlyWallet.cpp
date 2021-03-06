#include <lookup/LookupManager.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <wallet/ReadOnlyWallet.hpp>

using forge::lookup::LookupManager;
using forge::wallet::ReadOnlyWallet;
using forge::core::UMEntry;


ReadOnlyWallet::ReadOnlyWallet(std::unique_ptr<lookup::LookupManager>&& lookup)
    : lookup_(std::move(lookup))
{
    lookup_
        ->getClient()
        .getAddresses()
        .onValue([this](auto addresses) {
            for(auto&& addr : addresses) {
                this->addNewOwnedAddress(std::move(addr));
            }
        });
}

auto ReadOnlyWallet::addWatchOnlyAddress(std::string adr)
    -> void
{
    watched_addresses_.insert(std::move(adr));
}

auto ReadOnlyWallet::deleteWatchOnlyAddress(const std::string& adr)
    -> void
{
    watched_addresses_.erase(adr);
}

auto ReadOnlyWallet::addNewOwnedAddress(std::string adr)
    -> void
{
    owned_addresses_.insert(std::move(adr));
}

auto ReadOnlyWallet::getOwnedUMEntrys() const
    -> std::vector<UMEntry>
{
    std::vector<UMEntry> ret_vec;
    for(auto&& addr : owned_addresses_) {
        auto new_entrys = lookup_->getUMEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getWatchOnlyUMEntrys() const
    -> std::vector<UMEntry>
{
    std::vector<UMEntry> ret_vec;
    for(auto&& addr : watched_addresses_) {
        auto new_entrys = lookup_->getUMEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getAllWatchedUMEntrys() const
    -> std::vector<UMEntry>
{
    auto owned_entrys = getOwnedUMEntrys();
    auto watched_entrys = getWatchOnlyUMEntrys();

    owned_entrys.insert(std::begin(owned_entrys),
                        std::make_move_iterator(std::begin(watched_entrys)),
                        std::make_move_iterator(std::end(watched_entrys)));

    return owned_entrys;
}

auto ReadOnlyWallet::getOwnedUniqueEntrys() const
    -> std::vector<core::UniqueEntry>
{
    std::vector<core::UniqueEntry> ret_vec;
    for(auto&& addr : owned_addresses_) {
        auto new_entrys = lookup_->getUniqueEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getWatchOnlyUniqueEntrys() const
    -> std::vector<core::UniqueEntry>
{
    std::vector<core::UniqueEntry> ret_vec;
    for(auto&& addr : watched_addresses_) {
        auto new_entrys = lookup_->getUniqueEntrysOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getAllWatchedUniqueEntrys() const
    -> std::vector<core::UniqueEntry>
{
    auto owned_entrys = getOwnedUniqueEntrys();
    auto watched_entrys = getWatchOnlyUniqueEntrys();

    owned_entrys.insert(std::begin(owned_entrys),
                        std::make_move_iterator(std::begin(watched_entrys)),
                        std::make_move_iterator(std::end(watched_entrys)));

    return owned_entrys;
}

auto ReadOnlyWallet::getOwnedUtilityTokens() const
    -> std::vector<core::UtilityToken>
{
    std::vector<core::UtilityToken> ret_vec;
    for(auto&& addr : owned_addresses_) {
        auto new_entrys = lookup_->getUtilityTokensOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getWatchOnlyUtilityTokens() const
    -> std::vector<core::UtilityToken>
{
    std::vector<core::UtilityToken> ret_vec;
    for(auto&& addr : watched_addresses_) {
        auto new_entrys = lookup_->getUtilityTokensOfOwner(addr);
        ret_vec.insert(std::end(ret_vec),
                       std::make_move_iterator(std::begin(new_entrys)),
                       std::make_move_iterator(std::end(new_entrys)));
    }

    return ret_vec;
}

auto ReadOnlyWallet::getAllWatchedUtilityTokens() const
    -> std::vector<core::UtilityToken>
{
    auto owned_entrys = getOwnedUtilityTokens();
    auto watched_entrys = getWatchOnlyUtilityTokens();

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

auto ReadOnlyWallet::ownesAddress(const std::string& addr) const
    -> bool
{
    return owned_addresses_.find(addr) != owned_addresses_.end();
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
