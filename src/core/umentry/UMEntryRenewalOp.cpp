#include <core/umentry/UMEntry.hpp>
#include <core/umentry/UMEntryRenewalOp.hpp>
#include <core/FlagIndexes.hpp>
#include <core/umentry/UMEntryOperation.hpp>

using forge::core::UMEntry;
using forge::core::UMEntryRenewalOp;
using forge::core::ENTRY_RENEWAL_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UMEntryRenewalOp::UMEntryRenewalOp(UMEntry&& entry,
                               std::string&& owner,
                               std::int64_t block,
                               std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UMEntryRenewalOp::getUMEntryKey() const
    -> const UMEntryKey&
{
    return entry_.first;
}

auto UMEntryRenewalOp::getUMEntryKey()
    -> UMEntryKey&
{
    return entry_.first;
}

auto UMEntryRenewalOp::getUMEntryValue() const
    -> const UMEntryValue&
{
    return entry_.second;
}

auto UMEntryRenewalOp::getUMEntry() const
    -> const UMEntry&
{
    return entry_;
}
auto UMEntryRenewalOp::getUMEntry()
    -> UMEntry&
{
    return entry_;
}

auto UMEntryRenewalOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UMEntryRenewalOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UMEntryRenewalOp::getUMEntryValue()
    -> UMEntryValue&
{
    return entry_.second;
}

auto UMEntryRenewalOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UMEntryRenewalOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createUMEntryRenewalOpMetadata(UMEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_RENEWAL_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
