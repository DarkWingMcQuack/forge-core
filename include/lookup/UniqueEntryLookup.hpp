#pragma once

#include <core/Coin.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>
#include <lookup/LookupError.hpp>
#include <map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::lookup {

class UniqueEntryLookup final
{
public:
    UniqueEntryLookup(std::int64_t start_block);
    UniqueEntryLookup();

    auto executeOperations(std::vector<core::UniqueEntryOperation>&& ops)
        -> utilxx::Result<void, LookupError>;

    auto lookup(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::UniqueEntryValue>>;

    auto lookup(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<core::UniqueEntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupOwner(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::int64_t>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupUniqueEntry(const core::EntryKey& key)
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<core::UniqueEntryValue>,
                       std::reference_wrapper<std::string>,
                       std::reference_wrapper<std::int64_t>>>;

    auto lookupUniqueEntry(const core::EntryKey& key) const
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<const core::UniqueEntryValue>,
                       std::reference_wrapper<const std::string>,
                       std::reference_wrapper<const std::int64_t>>>;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    auto removeUniqueEntrysOlderThan(std::int64_t blocks)
        -> void;

    auto isCurrentlyValid(const core::UniqueEntryOperation& op) const
        -> bool;

    auto filterNonRelevantOperations(std::vector<core::UniqueEntryOperation>&& ops) const
        -> std::vector<core::UniqueEntryOperation>;

    auto clear()
        -> void;

    auto getUniqueEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::UniqueEntry>;

    auto operator()(core::UniqueEntryCreationOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UniqueEntryRenewalOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UniqueEntryOwnershipTransferOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UniqueEntryDeletionOp&& op)
        -> utilxx::Result<void, LookupError>;

private:
    using MapType = std::map<core::EntryKey, //key
                             std::tuple<core::UniqueEntryValue, //value
                                        std::string, //owner
                                        std::int64_t>>; //block
    MapType lookup_map_;
    std::int64_t block_height_;
    std::int64_t start_block_;
};

} // namespace forge::lookup
