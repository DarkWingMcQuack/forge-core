#pragma once

#include <entrys/umentry/UMEntry.hpp>

namespace forge::core {

class UMEntryOwnershipTransferOp
{
public:
    UMEntryOwnershipTransferOp(UMEntry&& entry,
                        std::string&& old_owner,
                        std::string&& new_owner,
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

    auto getNewOwner() const
        -> const std::string&;
    auto getNewOwner()
        -> std::string&;

    auto getUMEntry() const
        -> const UMEntry&;
    auto getUMEntry()
        -> UMEntry&;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

    auto getBlock() const
        -> std::int64_t;

    auto getValue() const
        -> std::int64_t;

private:
    UMEntry entry_;
    std::string old_owner_;
    std::string new_owner_;
    std::int64_t block_;
    std::int64_t value_;
};

auto createUMEntryOwnershipTransferOpMetadata(UMEntry&& entry)
    -> std::vector<std::byte>;


} // namespace forge::core
