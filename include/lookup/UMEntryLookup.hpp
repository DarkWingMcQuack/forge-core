#pragma once

#include <core/Coin.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <lookup/LookupError.hpp>
#include <map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::lookup {

class UMEntryLookup final
{
public:
    UMEntryLookup(std::int64_t start_block);
    UMEntryLookup();

    auto executeOperations(std::vector<core::UMEntryOperation>&& ops)
        -> void;

    auto lookup(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::UMEntryValue>>;

    auto lookup(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<core::UMEntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupOwner(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::int64_t>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupUMEntry(const core::EntryKey& key)
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<core::UMEntryValue>,
                       std::reference_wrapper<std::string>,
                       std::reference_wrapper<std::int64_t>>>;

    auto lookupUMEntry(const core::EntryKey& key) const
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<const core::UMEntryValue>,
                       std::reference_wrapper<const std::string>,
                       std::reference_wrapper<const std::int64_t>>>;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    auto removeUMEntrysOlderThan(std::int64_t diff)
        -> void;

    auto isCurrentlyValid(const core::UMEntryOperation& op) const
        -> bool;

    auto filterNonRelevantOperations(std::vector<core::UMEntryOperation>&& ops) const
        -> std::vector<core::UMEntryOperation>;

    auto clear()
        -> void;

    auto getUMEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::UMEntry>;

    auto operator()(core::UMEntryCreationOp&& op)
        -> void;

    auto operator()(core::UMEntryRenewalOp&& op)
        -> void;

    auto operator()(core::UMEntryOwnershipTransferOp&& op)
        -> void;

    auto operator()(core::UMEntryUpdateOp&& op)
        -> void;

    auto operator()(core::UMEntryDeletionOp&& op)
        -> void;

private:
    using MapType = std::map<core::EntryKey, //key
                             std::tuple<core::UMEntryValue, //value
                                        std::string, //owner
                                        std::int64_t>>; //block
    MapType lookup_map_;
    std::int64_t block_height_;
    std::int64_t start_block_;
};

} // namespace forge::lookup
