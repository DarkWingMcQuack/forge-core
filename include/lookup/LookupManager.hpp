#pragma once

#include <core/Operation.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <functional>
#include <lookup/EntryLookup.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::lookup {

using ManagerError = std::variant<LookupError,
                                  daemon::DaemonError>;

class LookupManager final
{
public:
    LookupManager(std::unique_ptr<daemon::DaemonBase> daemon);

    auto updateLookup()
        -> util::Result<bool, ManagerError>;

    auto rebuildLookup()
        -> util::Result<void, ManagerError>;

    auto lookupValue(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const core::EntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const std::string>>;

    auto lookupIsValid() const
        -> util::Result<bool, daemon::DaemonError>;

private:
    auto processBlock(core::Block&& block)
        -> util::Result<void, ManagerError>;

private:
    std::unique_ptr<daemon::DaemonBase> daemon_;
    EntryLookup lookup_;
    std::vector<std::string> block_hashes_;
};

} // namespace buddy::lookup
