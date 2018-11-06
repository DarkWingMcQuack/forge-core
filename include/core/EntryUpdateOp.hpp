#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class EntryUpdateOp
{
public:
    EntryUpdateOp(Entry&& entry,
                  std::string&& owner,
                  std::size_t block,
                  std::size_t value);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getNewEntryValue() const
        -> const EntryValue&;

    auto getNewEntryValue()
        -> EntryValue&;

    auto getEntry() const
        -> const Entry&;
    auto getEntry()
        -> Entry&;

    auto getBlock() const
        -> std::size_t;

    auto getValue() const
        -> std::size_t;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

private:
    Entry entry_;
    std::string owner_;
    std::size_t block_;
    std::size_t value_;
};

} // namespace buddy::core
