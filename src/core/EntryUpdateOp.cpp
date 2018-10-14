#include <core/Entry.hpp>
#include <core/EntryUpdateOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryUpdateOp;

EntryUpdateOp::EntryUpdateOp(Entry&& entry,
                             std::string&& owner,
                             std::size_t block,
                             std::size_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryUpdateOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getNewEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryUpdateOp::getNewEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryUpdateOp::getBlock() const
    -> std::size_t
{
    return block_;
}

auto EntryUpdateOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryUpdateOp::getOwner()
    -> std::string&
{
    return owner_;
}
