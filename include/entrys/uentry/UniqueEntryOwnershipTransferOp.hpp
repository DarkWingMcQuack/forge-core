#pragma once

#include <entrys/uentry/UniqueEntry.hpp>

namespace forge::core {

class UniqueEntryOwnershipTransferOp
{
public:
    UniqueEntryOwnershipTransferOp(UniqueEntry&& entry,
                                   std::string&& old_owner,
                                   std::string&& new_owner,
                                   std::int64_t block,
                                   std::int64_t value);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getUniqueEntryValue() const
        -> const UniqueEntryValue&;
    auto getUniqueEntryValue()
        -> UniqueEntryValue&;

    auto getNewOwner() const
        -> const std::string&;
    auto getNewOwner()
        -> std::string&;

    auto getUniqueEntry() const
        -> const UniqueEntry&;
    auto getUniqueEntry()
        -> UniqueEntry&;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

    auto getBlock() const
        -> std::int64_t;

    auto getValue() const
        -> std::int64_t;

private:
    UniqueEntry entry_;
    std::string old_owner_;
    std::string new_owner_;
    std::int64_t block_;
    std::int64_t value_;
};

auto createUniqueEntryOwnershipTransferOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>;


} // namespace forge::core
