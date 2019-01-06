#include <core/Entry.hpp>
#include <core/EntryCreationOp.hpp>
#include <core/Operation.hpp>

using buddy::core::Entry;
using buddy::core::EntryCreationOp;
using buddy::core::ENTRY_CREATION_FLAG;
using buddy::core::BUDDY_IDENTIFIER_MASK;

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


auto buddy::core::createEntryCreationOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    auto data = buddy::core::entryToRawData(entry);
    auto flag = buddy::core::ENTRY_CREATION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data),
                std::begin(BUDDY_IDENTIFIER_MASK),
                std::end(BUDDY_IDENTIFIER_MASK));

    return data;
}
