#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class OwnershipTransferOp
{
public:
    OwnershipTransferOp(Entry&&,
                        std::string&& old_owner,
                        std::string&& new_owner,
                        std::size_t block,
                        std::size_t value);

    auto getEntryKey() const
        -> const EntryKey&;
    auto getEntryKey()
        -> EntryKey&;

    auto getEntryValue() const
        -> const EntryValue&;
    auto getEntryValue()
        -> EntryValue&;

    auto getNewOwner() const
        -> const std::string&;
    auto getNewOwner()
        -> std::string&;

    auto getOldOwner() const
        -> const std::string&;
    auto getOldOwner()
        -> std::string&;

    auto getBlock() const
        -> std::size_t;

    auto getValue() const
        -> std::size_t;

private:
    Entry entry_;
    std::string old_owner_;
    std::string new_owner_;
    std::size_t block_;
    std::size_t value_;
};

} // namespace buddy::core
