#pragma once

#include <core/Coin.hpp>
#include <core/Operation.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <functional>
#include <lookup/EntryLookup.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace buddy::lookup {

using ManagerError = std::variant<LookupError,
                                  daemon::DaemonError>;

class LookupManager final
{
public:
    LookupManager(std::unique_ptr<daemon::ReadOnlyDaemonBase> daemon);

    auto updateLookup()
        -> utilxx::Result<bool, ManagerError>;

    auto rebuildLookup()
        -> utilxx::Result<void, ManagerError>;

    auto lookupValue(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::EntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupIsValid() const
        -> utilxx::Result<bool, daemon::DaemonError>;

    auto getEntryLookup() const
        -> const EntryLookup&;

    auto getEntryLookup()
        -> EntryLookup&;

private:
    auto processBlock(core::Block&& block)
        -> utilxx::Result<void, ManagerError>;

private:
    std::unique_ptr<daemon::ReadOnlyDaemonBase> daemon_;
    EntryLookup lookup_;
    std::vector<std::string> block_hashes_;
};

} // namespace buddy::lookup
