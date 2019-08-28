#include <core/FlagIndexes.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/uentry/UniqueEntryCreationOp.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>

using forge::core::UniqueEntry;
using forge::core::UniqueEntryCreationOp;
using forge::core::UNIQUE_ENTRY_CREATION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UniqueEntryCreationOp::UniqueEntryCreationOp(UniqueEntry&& entry,
                                             std::string&& owner,
                                             std::int64_t block,
                                             std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UniqueEntryCreationOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryCreationOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryCreationOp::getUniqueEntryValue() const
    -> const UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryCreationOp::getUniqueEntryValue()
    -> UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryCreationOp::getUniqueEntry() const
    -> const UniqueEntry&
{
    return entry_;
}
auto UniqueEntryCreationOp::getUniqueEntry()
    -> UniqueEntry&
{
    return entry_;
}

auto UniqueEntryCreationOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UniqueEntryCreationOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UniqueEntryCreationOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UniqueEntryCreationOp::getOwner()
    -> std::string&
{
    return owner_;
}


auto forge::core::createUniqueEntryCreationOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::uniqueEntryToRawData(entry);
    auto flag = forge::core::UNIQUE_ENTRY_CREATION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UNIQUE_ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
