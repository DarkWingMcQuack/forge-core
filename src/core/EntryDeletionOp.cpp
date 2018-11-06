#include <core/Entry.hpp>
#include <core/EntryDeletionOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryDeletionOp;

EntryDeletionOp::EntryDeletionOp(Entry&& entry,
                                 std::string&& owner,
                                 std::size_t block,
                                 std::size_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryDeletionOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryDeletionOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryDeletionOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryDeletionOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryDeletionOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto EntryDeletionOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto EntryDeletionOp::getBlock() const
    -> std::size_t
{
    return block_;
}

auto EntryDeletionOp::getValue() const
    -> std::size_t
{
    return value_;
}

auto EntryDeletionOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryDeletionOp::getOwner()
    -> std::string&
{
    return owner_;
}
