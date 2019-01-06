#pragma once

#include <core/Coin.hpp>
#include <core/Operation.hpp>
#include <functional>
#include <map>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace buddy::lookup {

class LookupError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


class EntryLookup final
{
public:
    EntryLookup(std::int64_t start_block);
    EntryLookup();

    auto executeOperations(std::vector<core::Operation>&& ops)
        -> utilxx::Result<void, LookupError>;

    auto lookup(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const core::EntryValue>>;

    auto lookup(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<core::EntryValue>>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::string>>;

    auto lookupOwner(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key)
        -> utilxx::Opt<std::reference_wrapper<std::int64_t>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupEntry(const core::EntryKey& key)
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<core::EntryValue>,
                       std::reference_wrapper<std::string>,
                       std::reference_wrapper<std::int64_t>>>;

    auto lookupEntry(const core::EntryKey& key) const
        -> utilxx::Opt<
            std::tuple<std::reference_wrapper<const core::EntryValue>,
                       std::reference_wrapper<const std::string>,
                       std::reference_wrapper<const std::int64_t>>>;

    auto setBlockHeight(std::int64_t height)
        -> void;

    auto getBlockHeight() const
        -> std::int64_t;

    auto removeEntrysOlderThan(std::int64_t blocks)
        -> void;

    auto isCurrentlyValid(const core::Operation& op) const
        -> bool;

    auto filterNonRelevantOperations(std::vector<core::Operation>&& ops) const
        -> std::vector<core::Operation>;

    auto clear()
        -> void;

    auto getEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::Entry>;

    auto operator()(core::EntryCreationOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::EntryRenewalOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::OwnershipTransferOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::EntryUpdateOp&& op)
        -> utilxx::Result<void, LookupError>;

    auto operator()(core::EntryDeletionOp&& op)
        -> utilxx::Result<void, LookupError>;

private:
    using MapType = std::map<core::EntryKey, //key
                             std::tuple<core::EntryValue, //value
                                        std::string, //owner
                                        std::int64_t>>; //block
    MapType lookup_map_;
    std::int64_t block_height_;
    std::int64_t start_block_;
};

} // namespace buddy::lookup
