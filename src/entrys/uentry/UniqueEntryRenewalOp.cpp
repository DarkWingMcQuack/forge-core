#include <core/FlagIndexes.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>
#include <entrys/uentry/UniqueEntryRenewalOp.hpp>

using forge::core::UniqueEntry;
using forge::core::UniqueEntryRenewalOp;
using forge::core::UNIQUE_ENTRY_RENEWAL_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UniqueEntryRenewalOp::UniqueEntryRenewalOp(UniqueEntry&& entry,
                                           std::string&& owner,
                                           std::int64_t block,
                                           std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UniqueEntryRenewalOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryRenewalOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryRenewalOp::getUniqueEntryValue() const
    -> const UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryRenewalOp::getUniqueEntry() const
    -> const UniqueEntry&
{
    return entry_;
}
auto UniqueEntryRenewalOp::getUniqueEntry()
    -> UniqueEntry&
{
    return entry_;
}

auto UniqueEntryRenewalOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UniqueEntryRenewalOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UniqueEntryRenewalOp::getUniqueEntryValue()
    -> UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryRenewalOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UniqueEntryRenewalOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createUniqueEntryRenewalOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::uniqueEntryToRawData(entry);
    auto flag = forge::core::UNIQUE_ENTRY_RENEWAL_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UNIQUE_ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
