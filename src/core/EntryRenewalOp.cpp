#include <core/Entry.hpp>
#include <core/EntryRenewalOp.hpp>
#include <core/Operation.hpp>

using forge::core::Entry;
using forge::core::EntryRenewalOp;
using forge::core::ENTRY_RENEWAL_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

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

auto forge::core::createEntryRenewalOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_RENEWAL_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
