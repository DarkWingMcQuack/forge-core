#include <core/umentry/UMEntry.hpp>
#include <core/umentry/UMEntryUpdateOp.hpp>
#include <core/FlagIndexes.hpp>
#include <core/umentry/UMEntryOperation.hpp>

using forge::core::UMEntry;
using forge::core::UMEntryKey;
using forge::core::UMEntryValue;
using forge::core::UMEntryUpdateOp;
using forge::core::ENTRY_UPDATE_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UMEntryUpdateOp::UMEntryUpdateOp(UMEntry&& entry,
                             std::string&& owner,
                             std::int64_t block,
                             std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UMEntryUpdateOp::getUMEntryKey() const
    -> const UMEntryKey&
{
    return entry_.first;
}

auto UMEntryUpdateOp::getUMEntryKey()
    -> UMEntryKey&
{
    return entry_.first;
}

auto UMEntryUpdateOp::getNewUMEntryValue() const
    -> const UMEntryValue&
{
    return entry_.second;
}

auto UMEntryUpdateOp::getNewUMEntryValue()
    -> UMEntryValue&
{
    return entry_.second;
}

auto UMEntryUpdateOp::getUMEntry() const
    -> const UMEntry&
{
    return entry_;
}
auto UMEntryUpdateOp::getUMEntry()
    -> UMEntry&
{
    return entry_;
}

auto UMEntryUpdateOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UMEntryUpdateOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UMEntryUpdateOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UMEntryUpdateOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createUMEntryUpdateOpMetadata(UMEntryKey&& key,
                                              UMEntryValue new_value)
    -> std::vector<std::byte>
{
    auto entry = UMEntry{std::move(key),
                       std::move(new_value)};
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_UPDATE_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
