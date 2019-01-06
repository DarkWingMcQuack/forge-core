#pragma once

#include <lookup/LookupManager.hpp>
#include <memory>
#include <string>
#include <vector>

namespace buddy::wallet {

class ReadOnlyWallet
{
public:
    auto update()
        -> void;

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
        -> const std::vector<std::string>&;

    auto getOwnedAddresses() const
        -> const std::vector<std::string>&;

protected:
    std::vector<std::string> owned_addresses_;
    std::vector<std::string> watched_addresses_;
    std::unique_ptr<lookup::LookupManager> lookup_;
};

} // namespace buddy::wallet
