#pragma once

#include <core/umentry/UMEntry.hpp>

namespace forge::core {

class UMEntryCreationOp
{
public:
    UMEntryCreationOp(UMEntry&& entry,
                    std::string&& owner,
                    std::int64_t block,
                    std::int64_t value);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getUMEntryValue() const
        -> const UMEntryValue&;
    auto getUMEntryValue()
        -> UMEntryValue&;

    auto getUMEntry() const
        -> const UMEntry&;
    auto getUMEntry()
        -> UMEntry&;

    auto getValue() const
        -> std::int64_t;

    auto getBlock() const
        -> std::int64_t;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

private:
    UMEntry entry_;
    std::string owner_;
    std::int64_t block_;
    std::int64_t value_;
};

auto createUMEntryCreationOpMetadata(UMEntry&& entry)
    -> std::vector<std::byte>;

} // namespace forge::core
