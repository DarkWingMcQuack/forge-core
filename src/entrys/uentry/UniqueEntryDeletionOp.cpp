#include <core/FlagIndexes.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/uentry/UniqueEntryDeletionOp.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>

using forge::core::UniqueEntry;
using forge::core::UniqueEntryDeletionOp;
using forge::core::UNIQUE_ENTRY_DELETION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UniqueEntryDeletionOp::UniqueEntryDeletionOp(UniqueEntry&& entry,
                                             std::string&& owner,
                                             std::int64_t block,
                                             std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UniqueEntryDeletionOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryDeletionOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryDeletionOp::getUniqueEntryValue() const
    -> const UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryDeletionOp::getUniqueEntryValue()
    -> UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryDeletionOp::getUniqueEntry() const
    -> const UniqueEntry&
{
    return entry_;
}
auto UniqueEntryDeletionOp::getUniqueEntry()
    -> UniqueEntry&
{
    return entry_;
}

auto UniqueEntryDeletionOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UniqueEntryDeletionOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UniqueEntryDeletionOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UniqueEntryDeletionOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createUniqueEntryDeletionOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = entry.toRawData();
    auto flag = forge::core::UNIQUE_ENTRY_DELETION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UNIQUE_ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
