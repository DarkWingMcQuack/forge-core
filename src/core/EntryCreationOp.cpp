#include <core/Entry.hpp>
#include <core/EntryCreationOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryCreationOp;

EntryCreationOp::EntryCreationOp(Entry&& entry,
                                 std::string&& owner,
                                 std::size_t block)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block) {}


auto EntryCreationOp::getKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryCreationOp::getValue()
    -> EntryValue&
{
    return entry_.second;
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
