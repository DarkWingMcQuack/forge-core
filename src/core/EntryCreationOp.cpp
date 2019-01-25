#include <core/Entry.hpp>
#include <core/EntryCreationOp.hpp>
#include <core/FlagIndexes.hpp>
#include <core/Operation.hpp>

using forge::core::Entry;
using forge::core::EntryCreationOp;
using forge::core::ENTRY_CREATION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

EntryCreationOp::EntryCreationOp(Entry&& entry,
                                 std::string&& owner,
                                 std::int64_t block,
                                 std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryCreationOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryCreationOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryCreationOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryCreationOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto EntryCreationOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto EntryCreationOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto EntryCreationOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto EntryCreationOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryCreationOp::getOwner()
    -> std::string&
{
    return owner_;
}


auto forge::core::createEntryCreationOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_CREATION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
