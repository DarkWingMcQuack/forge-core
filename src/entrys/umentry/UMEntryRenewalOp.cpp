#include <core/FlagIndexes.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <entrys/umentry/UMEntryRenewalOp.hpp>

using forge::core::UMEntry;
using forge::core::UMEntryRenewalOp;
using forge::core::UMENTRY_RENEWAL_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UMEntryRenewalOp::UMEntryRenewalOp(UMEntry&& entry,
                                   std::string&& owner,
                                   std::int64_t block,
                                   std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UMEntryRenewalOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UMEntryRenewalOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UMEntryRenewalOp::getUMEntryValue() const
    -> const UMEntryValue&
{
    return entry_.getValue();
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
    return entry_.getValue();
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
    auto data = forge::core::umEntryToRawData(entry);
    auto flag = forge::core::UMENTRY_RENEWAL_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UMENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
