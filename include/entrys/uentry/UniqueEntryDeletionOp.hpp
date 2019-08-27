#pragma once

#include <entrys/uentry/UniqueEntry.hpp>

namespace forge::core {

class UniqueEntryDeletionOp
{
public:
    UniqueEntryDeletionOp(UniqueEntry&& entry,
                          std::string&& owner,
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

    auto getUniqueEntry() const
        -> const UniqueEntry&;
    auto getUniqueEntry()
        -> UniqueEntry&;

    auto getBlock() const
        -> std::int64_t;

    auto getValue() const
        -> std::int64_t;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

private:
    UniqueEntry entry_;
    std::string owner_;
    std::int64_t block_;
    std::int64_t value_;
};


auto createUniqueEntryDeletionOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>;

} // namespace forge::core
