#pragma once

#include <core/Coin.hpp>
#include <core/umentry/UMEntryOperation.hpp>
#include <functional>
#include <map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::lookup {

class LookupError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


class UMEntryLookup final
{
public:
    UMEntryLookup(std::int64_t start_block);
    UMEntryLookup();

    auto executeOperations(std::vector<core::UMEntryOperation>&& ops)
        -> utilxx::Result<void, LookupError>;

    auto lookup(const core::UMEntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::UMEntryValue>>;

    auto lookup(const core::UMEntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<core::UMEntryValue>>;

    auto lookupOwner(const core::UMEntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupOwner(const core::UMEntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::string>>;

    auto lookupActivationBlock(const core::UMEntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::int64_t>>;

    auto lookupActivationBlock(const core::UMEntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupUMEntry(const core::UMEntryKey& key)
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<core::UMEntryValue>,
                       std::reference_wrapper<std::string>,
                       std::reference_wrapper<std::int64_t>>>;

    auto lookupUMEntry(const core::UMEntryKey& key) const
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<const core::UMEntryValue>,
                       std::reference_wrapper<const std::string>,
                       std::reference_wrapper<const std::int64_t>>>;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    auto removeUMEntrysOlderThan(std::int64_t blocks)
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
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UMEntryRenewalOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UMEntryOwnershipTransferOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UMEntryUpdateOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::UMEntryDeletionOp&& op)
        -> utilxx::Result<void, LookupError>;

private:
    using MapType = std::map<core::UMEntryKey, //key
                             std::tuple<core::UMEntryValue, //value
                                        std::string, //owner
                                        std::int64_t>>; //block
    MapType lookup_map_;
    std::int64_t block_height_;
    std::int64_t start_block_;
};

} // namespace forge::lookup
