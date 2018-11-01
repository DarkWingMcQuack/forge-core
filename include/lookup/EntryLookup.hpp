#pragma once

#include <core/Operation.hpp>
#include <daemon/Coin.hpp>
#include <functional>
#include <map>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::lookup {

class LookupError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


class EntryLookup final
{
public:
    EntryLookup(std::size_t start_block);
    EntryLookup();

    auto executeOperations(std::vector<core::Operation>&& ops)
        -> util::Result<void, LookupError>;

    auto lookup(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const core::EntryValue>>;

    auto lookup(const core::EntryKey& key)
        -> util::Opt<std::reference_wrapper<core::EntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> util::Opt<std::reference_wrapper<const std::string>>;

    auto lookupOwner(const core::EntryKey& key)
        -> util::Opt<std::reference_wrapper<std::string>>;

    auto lookupBlock(const core::EntryKey& key)
        -> util::Opt<std::reference_wrapper<std::size_t>>;

    auto lookupEntry(const core::EntryKey& key)
        -> util::Opt<
            std::tuple<std::reference_wrapper<core::EntryValue>,
                       std::reference_wrapper<std::string>,
                       std::reference_wrapper<std::size_t>>>;

    auto lookupEntry(const core::EntryKey& key) const
        -> util::Opt<
            std::tuple<std::reference_wrapper<const core::EntryValue>,
                       std::reference_wrapper<const std::string>,
                       std::reference_wrapper<const std::size_t>>>;

    auto setBlockHeight(std::size_t height)
        -> void;

    auto getBlockHeight() const
        -> std::size_t;

    auto removeEntrysOlderThan(std::size_t blocks)
        -> void;

    auto isCurrentlyValid(const core::Operation& op) const
        -> bool;

    auto filterNonRelevantOperations(std::vector<core::Operation>&& ops) const
        -> std::vector<core::Operation>;

    auto clear()
        -> void;

    auto operator()(core::EntryCreationOp&& op)
        -> util::Result<void, LookupError>;

    auto operator()(core::EntryRenewalOp&& op)
        -> util::Result<void, LookupError>;

    auto operator()(core::OwnershipTransferOp&& op)
        -> util::Result<void, LookupError>;

    auto operator()(core::EntryUpdateOp&& op)
        -> util::Result<void, LookupError>;

    auto operator()(core::EntryDeletionOp&& op)
        -> util::Result<void, LookupError>;

private:
    using MapType = std::map<core::EntryKey, //key
                             std::tuple<core::EntryValue, //value
                                        std::string, //owner
                                        std::size_t>>; //block
    MapType lookup_map_;
    std::size_t block_height_;
    std::size_t start_block_;
};

} // namespace buddy::lookup
