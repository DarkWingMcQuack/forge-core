#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class EntryRenewalOp
{
public:
    EntryRenewalOp(Entry&& entry,
                   std::string&& owner,
                   std::size_t block);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getEntryValue() const
        -> const EntryValue&;
    auto getEntryValue()
        -> EntryValue&;

    auto getBlock() const
        -> std::size_t;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

private:
    Entry entry_;
    std::string owner_;
    std::size_t block_;
};

} // namespace buddy::core
