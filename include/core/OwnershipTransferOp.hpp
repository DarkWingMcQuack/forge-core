#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class OwnershipTransferOp
{
public:
    OwnershipTransferOp(Entry&&,
                        std::string&& old_owner,
                        std::string&& new_owner,
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

    auto getNewOwner() const
        -> const std::string&;
    auto getNewOwner()
        -> std::string&;

    auto getEntry() const
        -> const Entry&;
    auto getEntry()
        -> Entry&;

    auto getOwner() const
        -> const std::string&;
    auto getOwner()
        -> std::string&;

    auto getBlock() const
        -> std::int64_t;

    auto getValue() const
        -> std::int64_t;

private:
    Entry entry_;
    std::string old_owner_;
    std::string new_owner_;
    std::int64_t block_;
    std::int64_t value_;
};

} // namespace buddy::core
