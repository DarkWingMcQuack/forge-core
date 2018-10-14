#include <core/Entry.hpp>
#include <core/EntryCreationOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryCreationOp;

EntryCreationOp::EntryCreationOp(Entry&& entry,
                                 std::string&& owner,
                                 std::size_t block,
                                 std::size_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryCreationOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryCreationOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryCreationOp::getValue() const
    -> std::size_t
{
    return value_;
}

auto EntryCreationOp::getBlock() const
    -> std::size_t
{
    return block_;
}

auto EntryCreationOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryCreationOp::getOwner()
    -> std::string&
{
    return owner_;
}
