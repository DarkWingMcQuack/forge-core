#include <core/Entry.hpp>
#include <core/EntryUpdateOp.hpp>
#include <core/FlagIndexes.hpp>
#include <core/Operation.hpp>

using forge::core::Entry;
using forge::core::EntryKey;
using forge::core::EntryValue;
using forge::core::EntryUpdateOp;
using forge::core::ENTRY_UPDATE_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

EntryUpdateOp::EntryUpdateOp(Entry&& entry,
                             std::string&& owner,
                             std::int64_t block,
                             std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryUpdateOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryUpdateOp::getNewEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryUpdateOp::getNewEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryUpdateOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto EntryUpdateOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto EntryUpdateOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto EntryUpdateOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto EntryUpdateOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryUpdateOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createEntryUpdateOpMetadata(EntryKey&& key,
                                              EntryValue new_value)
    -> std::vector<std::byte>
{
    auto entry = Entry{std::move(key),
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
