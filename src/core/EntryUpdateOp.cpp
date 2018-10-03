#include <core/Entry.hpp>
#include <core/EntryUpdateOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryUpdateOp;

EntryUpdateOp::EntryUpdateOp(Entry&& entry,
                             std::string&& owner,
                             std::size_t block)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block) {}


auto EntryUpdateOp::getKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getNewValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryUpdateOp::getNewValue()
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
