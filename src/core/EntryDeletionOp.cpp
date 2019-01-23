#include <core/Entry.hpp>
#include <core/EntryDeletionOp.hpp>
#include <core/Operation.hpp>

using forge::core::Entry;
using forge::core::EntryDeletionOp;
using forge::core::ENTRY_DELETION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

EntryDeletionOp::EntryDeletionOp(Entry&& entry,
                                 std::string&& owner,
                                 std::int64_t block,
                                 std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto EntryDeletionOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto EntryDeletionOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto EntryDeletionOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto EntryDeletionOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto EntryDeletionOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto EntryDeletionOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto EntryDeletionOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto EntryDeletionOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto EntryDeletionOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto EntryDeletionOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createEntryDeletionOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_DELETION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
