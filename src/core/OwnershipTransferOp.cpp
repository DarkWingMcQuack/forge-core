#include <core/Entry.hpp>
#include <core/FlagIndexes.hpp>
#include <core/Operation.hpp>
#include <core/OwnershipTransferOp.hpp>

using forge::core::Entry;
using forge::core::OwnershipTransferOp;
using forge::core::OWNERSHIP_TRANSFER_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

OwnershipTransferOp::OwnershipTransferOp(Entry&& entry,
                                         std::string&& old_owner,
                                         std::string&& new_owner,
                                         std::int64_t block,
                                         std::int64_t value)
    : entry_(std::move(entry)),
      old_owner_(std::move(old_owner)),
      new_owner_(std::move(new_owner)),
      block_(block),
      value_(value) {}


auto OwnershipTransferOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.first;
}

auto OwnershipTransferOp::getEntryKey()
    -> EntryKey&
{
    return entry_.first;
}

auto OwnershipTransferOp::getEntryValue() const
    -> const EntryValue&
{
    return entry_.second;
}

auto OwnershipTransferOp::getEntryValue()
    -> EntryValue&
{
    return entry_.second;
}

auto OwnershipTransferOp::getEntry() const
    -> const Entry&
{
    return entry_;
}
auto OwnershipTransferOp::getEntry()
    -> Entry&
{
    return entry_;
}

auto OwnershipTransferOp::getNewOwner() const
    -> const std::string&
{
    return new_owner_;
}

auto OwnershipTransferOp::getNewOwner()
    -> std::string&
{
    return new_owner_;
}

auto OwnershipTransferOp::getOwner() const
    -> const std::string&
{
    return old_owner_;
}

auto OwnershipTransferOp::getOwner()
    -> std::string&
{
    return old_owner_;
}

auto OwnershipTransferOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto OwnershipTransferOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto forge::core::createOwnershipTransferOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::OWNERSHIP_TRANSFER_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
