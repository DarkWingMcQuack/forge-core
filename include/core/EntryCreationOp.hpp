#pragma once

#include <core/Entry.hpp>

namespace forge::core {

class EntryCreationOp
{
public:
    EntryCreationOp(Entry&& entry,
                    std::string&& owner,
                    std::int64_t block,
                    std::int64_t value);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getEntryValue() const
        -> const EntryValue&;
    auto getEntryValue()
        -> EntryValue&;

    auto getEntry() const
        -> const Entry&;
    auto getEntry()
        -> Entry&;

    auto getValue() const
        -> std::int64_t;

    auto getBlock() const
        -> std::int64_t;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

private:
    Entry entry_;
    std::string owner_;
    std::int64_t block_;
    std::int64_t value_;
};

auto createEntryCreationOpMetadata(Entry&& entry)
    -> std::vector<std::byte>;

} // namespace forge::core
