#include <core/Entry.hpp>
#include <core/EntryRenewalOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryRenewalOp;

EntryRenewalOp::EntryRenewalOp(Entry&& entry,
                               std::string&& owner,
                               std::int64_t block,
                               std::int64_t value)
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

auto EntryRenewalOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto EntryRenewalOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto EntryRenewalOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto EntryRenewalOp::getBlock() const
    -> std::int64_t
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
