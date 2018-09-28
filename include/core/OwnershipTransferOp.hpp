#pragma once

#include <core/Entry.hpp>

namespace buddy::core {

class OwnershipTransferOp
{
public:
    OwnershipTransferOp(Entry&&,
                        std::string&& old_owner,
                        std::string&& new_owner,
                        std::size_t block);

    auto getKey() const
        -> const EntryKey&;
    auto getKey()
        -> EntryKey&;

    auto getValue() const
        -> const EntryValue&;
    auto getValue()
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

private:
    Entry entry_;
    std::string old_owner_;
    std::string new_owner_;
    std::size_t block_;
};

} // namespace buddy::core
