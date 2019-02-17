#pragma once

#include <core/Coin.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <functional>
#include <lookup/UMEntryLookup.hpp>
#include <shared_mutex>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::lookup {

using ManagerError = std::variant<LookupError,
                                  daemon::DaemonError>;

auto generateMessage(ManagerError&& error)
    -> std::string;

class LookupManager final
{
public:
    LookupManager(std::unique_ptr<daemon::ReadOnlyDaemonBase>&& daemon);

    LookupManager(LookupManager&&) = default;

    auto operator=(LookupManager &&)
        -> LookupManager& = default;

    auto updateLookup()
        -> utilxx::Result<bool, ManagerError>;

    auto rebuildLookup()
        -> utilxx::Result<void, ManagerError>;

    auto lookupValue(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::UMEntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupIsValid() const
        -> utilxx::Result<bool, daemon::DaemonError>;

    auto getLastValidBlockHeight() const
        -> utilxx::Result<int64_t, daemon::DaemonError>;

    auto getUMEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::UMEntry>;

    auto getCoin() const
        -> core::Coin;

    auto getDaemon() const
        -> const daemon::ReadOnlyDaemonBase&;

private:
    auto processBlock(core::Block&& block)
        -> utilxx::Result<void, ManagerError>;

private:
    std::unique_ptr<daemon::ReadOnlyDaemonBase> daemon_;

    mutable std::shared_mutex rw_mtx_;
    UMEntryLookup lookup_;
    std::vector<std::string> block_hashes_;
};

} // namespace forge::lookup
