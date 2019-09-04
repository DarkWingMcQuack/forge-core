#pragma once

#include <lookup/LookupManager.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace forge::wallet {

class ReadOnlyWallet
{
public:
    ReadOnlyWallet(std::unique_ptr<lookup::LookupManager>&& lookup);

    auto addWatchOnlyAddress(std::string adr)
        -> void;

    auto deleteWatchOnlyAddress(const std::string& adr)
        -> void;

    auto addNewOwnedAddress(std::string adr)
        -> void;

    auto getOwnedUMEntrys() const
        -> std::vector<core::UMEntry>;

    auto getWatchOnlyUMEntrys() const
        -> std::vector<core::UMEntry>;

    auto getAllWatchedUMEntrys() const
        -> std::vector<core::UMEntry>;

    auto getOwnedUniqueEntrys() const
        -> std::vector<core::UniqueEntry>;

    auto getWatchOnlyUniqueEntrys() const
        -> std::vector<core::UniqueEntry>;

    auto getAllWatchedUniqueEntrys() const
        -> std::vector<core::UniqueEntry>;

    auto getOwnedUtilityTokens() const
        -> std::vector<core::UtilityToken>;

    auto getWatchOnlyUtilityTokens() const
        -> std::vector<core::UtilityToken>;

    auto getAllWatchedUtilityTokens() const
        -> std::vector<core::UtilityToken>;

    auto getWatchedAddresses() const
        -> const std::set<std::string>&;

    auto getOwnedAddresses() const
        -> const std::set<std::string>&;

    auto ownesAddress(const std::string& addr) const
        -> bool;

    auto getLookup() const
        -> const lookup::LookupManager&;
    auto getLookup()
        -> lookup::LookupManager&;

protected:
    std::set<std::string> owned_addresses_;
    std::set<std::string> watched_addresses_;
    std::unique_ptr<lookup::LookupManager> lookup_;
};

} // namespace forge::wallet
