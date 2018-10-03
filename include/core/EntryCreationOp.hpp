#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class EntryCreationOp
{
public:
    EntryCreationOp(Entry&& entry,
                    std::string&& owner,
                    std::size_t block,
                    std::size_t value);

    auto getKey() const
        -> const EntryKey&;
    auto getKey()
        -> EntryKey&;

    auto getValue() const
        -> const EntryValue&;
    auto getValue()
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
    std::size_t value_;
};

} // namespace buddy::core
