#pragma once

#include <core/Operation.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <functional>
#include <lookup/EntryLookup.hpp>
#include <unordered_map>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::lookup {

using ManagerError = std::variant<LookupError,
                                  daemon::DaemonError>;

class LookupManager final
{
public:
    auto updateLookup()
        -> util::Result<void, ManagerError>;

    auto rebuildLookup()
        -> util::Result<void, ManagerError>;

    auto lookupValue(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const core::EntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const std::string>>;

    auto lookupIsValid() const
        -> util::Result<bool, daemon::DaemonError>;

private:
    std::unique_ptr<daemon::DaemonBase> daemon_;
    EntryLookup lookup_;
    std::vector<std::vector<std::byte>> block_hashes_;
};

} // namespace buddy::lookup
