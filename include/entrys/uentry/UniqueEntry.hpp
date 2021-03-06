#pragma once

#include <array>
#include <cstddef>
#include <entrys/umentry/UMEntry.hpp>
#include <json/value.h>
#include <utils/Opt.hpp>
#include <variant>
#include <vector>

namespace forge::core {

//need to redefine here to avoid circular header
//dependency
using EntryKey = std::vector<std::byte>;

//flag which identifies a UMENTRY
constexpr static inline auto UNIQUE_ENTRY_IDENTIFICATION_FLAG = static_cast<std::byte>(0b00000010);

using UniqueEntryValue = std::variant<IPv4Value,
                                      IPv6Value,
                                      ByteArray,
                                      NoneValue>;

class UniqueEntry
{
public:
    UniqueEntry(EntryKey, UniqueEntryValue);

    auto toRawData() const
        -> std::vector<std::byte>;

    auto toJson() const
        -> Json::Value;

    auto getKey() const
        -> const EntryKey&;
    auto getKey()
        -> EntryKey&;

    auto getValue() const
        -> const UniqueEntryValue&;
    auto getValue()
        -> UniqueEntryValue&;

private:
    EntryKey key_;
    UniqueEntryValue value_;
};

auto parseUniqueValue(const std::vector<std::byte>& data)
    -> utils::Opt<UniqueEntryValue>;

auto parseUniqueKey(const std::vector<std::byte>& data)
    -> utils::Opt<EntryKey>;

auto parseUniqueEntry(const std::vector<std::byte>& data)
    -> utils::Opt<UniqueEntry>;

auto extractUniqueValueFlag(const UniqueEntryValue& value)
    -> std::byte;

auto uniqueEntryValueToRawData(const UniqueEntryValue& value)
    -> std::vector<std::byte>;

auto jsonToUniqueEntryValue(Json::Value&& value)
    -> utils::Opt<UniqueEntryValue>;

auto uniqueEntryValueToJson(UniqueEntryValue value)
    -> Json::Value;
} // namespace forge::core
