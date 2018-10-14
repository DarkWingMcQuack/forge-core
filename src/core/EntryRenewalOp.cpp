#include <core/Entry.hpp>
#include <core/EntryRenewalOp.hpp>

using buddy::core::Entry;
using buddy::core::EntryRenewalOp;

EntryRenewalOp::EntryRenewalOp(Entry&& entry,
                               std::string&& owner,
                               std::size_t block)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block) {}


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

auto EntryRenewalOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryRenewalOp::getBlock() const
    -> std::size_t
{
    return block_;
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
