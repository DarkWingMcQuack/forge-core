#pragma once

#include <lookup/LookupManager.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace buddy::wallet {

class ReadOnlyWallet
{
public:
    ReadOnlyWallet(std::unique_ptr<lookup::LookupManager>&& lookup);

    auto addWatchOnlyAddress(std::string&& adr)
        -> void;

    auto deleteWatchOnlyAddress(const std::string& adr)
        -> void;

    auto addNewOwnedAddress(std::string&& adr)
        -> void;

    auto getOwnedEntrys() const
        -> std::vector<core::Entry>;

    auto getWatchOnlyEntrys() const
        -> std::vector<core::Entry>;

    auto getAllWatchedEntrys() const
        -> std::vector<core::Entry>;

    auto getWatchedAddresses() const
        -> const std::set<std::string>&;

    auto getOwnedAddresses() const
        -> const std::set<std::string>&;

    auto getLookup() const
        -> const lookup::LookupManager&;
    auto getLookup()
        -> lookup::LookupManager&;

protected:
    std::set<std::string> owned_addresses_;
    std::set<std::string> watched_addresses_;
    std::unique_ptr<lookup::LookupManager> lookup_;
};

} // namespace buddy::wallet
