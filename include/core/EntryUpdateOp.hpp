#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class EntryUpdateOp
{
public:
    EntryUpdateOp(Entry&& entry,
                  std::string&& owner,
                  std::size_t block);

    auto getKey() const
        -> const EntryKey&;
    auto getKey()
        -> EntryKey&;

    auto getNewValue() const
        -> const EntryValue&;
    auto getNewValue()
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
