#include <core/umentry/UMEntry.hpp>
#include <core/umentry/UMEntryDeletionOp.hpp>
#include <core/FlagIndexes.hpp>
#include <core/umentry/UMEntryOperation.hpp>

using forge::core::UMEntry;
using forge::core::UMEntryDeletionOp;
using forge::core::ENTRY_DELETION_FLAG;
using forge::core::FORGE_IDENTIFIER_MASK;

UMEntryDeletionOp::UMEntryDeletionOp(UMEntry&& entry,
                                 std::string&& owner,
                                 std::int64_t block,
                                 std::int64_t value)
    : entry_(std::move(entry)),
      owner_(std::move(owner)),
      block_(block),
      value_(value) {}


auto UMEntryDeletionOp::getUMEntryKey() const
    -> const UMEntryKey&
{
    return entry_.first;
}

auto UMEntryDeletionOp::getUMEntryKey()
    -> UMEntryKey&
{
    return entry_.first;
}

auto UMEntryDeletionOp::getUMEntryValue() const
    -> const UMEntryValue&
{
    return entry_.second;
}

auto UMEntryDeletionOp::getUMEntryValue()
    -> UMEntryValue&
{
    return entry_.second;
}

auto UMEntryDeletionOp::getUMEntry() const
    -> const UMEntry&
{
    return entry_;
}
auto UMEntryDeletionOp::getUMEntry()
    -> UMEntry&
{
    return entry_;
}

auto UMEntryDeletionOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto UMEntryDeletionOp::getValue() const
    -> std::int64_t
{
    return value_;
}

auto UMEntryDeletionOp::getOwner() const
    -> const std::string&
{
    return owner_;
}
auto UMEntryDeletionOp::getOwner()
    -> std::string&
{
    return owner_;
}

auto forge::core::createUMEntryDeletionOpMetadata(UMEntry&& entry)
    -> std::vector<std::byte>
{
    auto data = forge::core::entryToRawData(entry);
    auto flag = forge::core::ENTRY_DELETION_FLAG;

    data.insert(std::begin(data), flag);

    data.insert(std::begin(data), ENTRY_IDENTIFICATION_FLAG);

    data.insert(std::begin(data),
                std::begin(FORGE_IDENTIFIER_MASK),
                std::end(FORGE_IDENTIFIER_MASK));

    return data;
}
