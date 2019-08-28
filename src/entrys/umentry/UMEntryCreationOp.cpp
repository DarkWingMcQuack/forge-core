#include <core/FlagIndexes.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryCreationOp.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>

using forge::core::UMEntry;
using forge::core::UMEntryCreationOp;
using forge::core::UMENTRY_CREATION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UMEntryCreationOp::UMEntryCreationOp(UMEntry&& entry,
                                     std::string&& owner,
                                     std::int64_t block,
                                     std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UMEntryCreationOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UMEntryCreationOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UMEntryCreationOp::getUMEntryValue() const
    -> const UMEntryValue&
{
    return entry_.getValue();
}

auto UMEntryCreationOp::getUMEntryValue()
    -> UMEntryValue&
{
    return entry_.getValue();
}

auto UMEntryCreationOp::getUMEntry() const
    -> const UMEntry&
{
    return entry_;
}
auto UMEntryCreationOp::getUMEntry()
    -> UMEntry&
{
    return entry_;
}

auto UMEntryCreationOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UMEntryCreationOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UMEntryCreationOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UMEntryCreationOp::getOwner()
    -> std::string&
{
    return owner_;
}


auto forge::core::createUMEntryCreationOpMetadata(UMEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = entry.toRawData();
    auto flag = forge::core::UMENTRY_CREATION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UMENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
