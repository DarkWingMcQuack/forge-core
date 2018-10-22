#include <core/Entry.hpp>
#include <core/EntryRenewalOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryRenewalOp;

EntryRenewalOp::EntryRenewalOp(Entry&& entry,
                               std::string&& owner,
                               std::size_t block,
                               std::size_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryRenewalOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryRenewalOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryRenewalOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryRenewalOp::getValue() const
    -> std::size_t
{
    return value_;
}

auto EntryRenewalOp::getBlock() const
    -> std::size_t
{
    return block_;
}

auto EntryRenewalOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryRenewalOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryRenewalOp::getOwner()
    -> std::string&
{
    return owner_;
}
