#include <core/FlagIndexes.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>
#include <entrys/uentry/UniqueEntryOwnershipTransferOp.hpp>

using forge::core::UniqueEntry;
using forge::core::UniqueEntryOwnershipTransferOp;
using forge::core::UNIQUE_ENTRY_OWNERSHIP_TRANSFER_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UniqueEntryOwnershipTransferOp::UniqueEntryOwnershipTransferOp(UniqueEntry&& entry,
                                                               std::string&& old_owner,
                                                               std::string&& new_owner,
                                                               std::int64_t block,
                                                               std::int64_t value)
    : entry_(std::move(entry)),
      old_owner_(std::move(old_owner)),
      new_owner_(std::move(new_owner)),
      block_(block),
      value_(value) {}


auto UniqueEntryOwnershipTransferOp::getEntryKey() const
    -> const EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryOwnershipTransferOp::getEntryKey()
    -> EntryKey&
{
    return entry_.getKey();
}

auto UniqueEntryOwnershipTransferOp::getUniqueEntryValue() const
    -> const UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryOwnershipTransferOp::getUniqueEntryValue()
    -> UniqueEntryValue&
{
    return entry_.getValue();
}

auto UniqueEntryOwnershipTransferOp::getUniqueEntry() const
    -> const UniqueEntry&
{
    return entry_;
}
auto UniqueEntryOwnershipTransferOp::getUniqueEntry()
    -> UniqueEntry&
{
    return entry_;
}

auto UniqueEntryOwnershipTransferOp::getNewOwner() const
    -> const std::string&
{
    return new_owner_;
}

auto UniqueEntryOwnershipTransferOp::getNewOwner()
    -> std::string&
{
    return new_owner_;
}

auto UniqueEntryOwnershipTransferOp::getOwner() const
    -> const std::string&
{
    return old_owner_;
}

auto UniqueEntryOwnershipTransferOp::getOwner()
    -> std::string&
{
    return old_owner_;
}

auto UniqueEntryOwnershipTransferOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UniqueEntryOwnershipTransferOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto forge::core::createUniqueEntryOwnershipTransferOpMetadata(UniqueEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::uniqueEntryToRawData(entry);
    auto flag = forge::core::UNIQUE_ENTRY_OWNERSHIP_TRANSFER_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), UNIQUE_ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
