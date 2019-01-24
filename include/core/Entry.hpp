#pragma once

#include <array>
#include <cstddef>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <vector>

namespace forge::core {

using EntryKey = std::vector<std::byte>;


// clang-format off
using IPv4Value = std::array<std::byte, 4>;
using IPv6Value = std::array<std::byte, 16>;
using ByteArray = std::vector<std::byte>;
struct NoneValue{
    auto operator==(const NoneValue&) const
    {
        return true;
    }
};
// clang-format on

constexpr static inline auto ENTRY_IDENTIFICATION_FLAG = static_cast<std::byte>(0b00000001);

constexpr static inline auto VALUE_FLAG_INDEX = 5;

constexpr static inline auto IPv4_VALUE_FLAG = static_cast<std::byte>(0b00000001);
constexpr static inline auto IPv6_VALUE_FLAG = static_cast<std::byte>(0b00000010);
constexpr static inline auto NONE_VALUE_FLAG = static_cast<std::byte>(0b00000100);
constexpr static inline auto BYTE_ARRAY_VALUE_FLAG = static_cast<std::byte>(0b00001000);

using EntryValue = std::variant<IPv4Value,
                                IPv6Value,
                                ByteArray,
                                NoneValue>;

using Entry = std::pair<EntryKey,
                        EntryValue>;

auto parseValue(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryValue>;

auto parseKey(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryKey>;

auto parseEntry(const std::vector<std::byte>& data)
    -> utilxx::Opt<Entry>;

auto extractValueFlag(const EntryValue& value)
    -> std::byte;

auto entryValueToRawData(const EntryValue& value)
    -> std::vector<std::byte>;

auto entryToRawData(const Entry& entry)
    -> std::vector<std::byte>;

auto jsonToEntryValue(Json::Value&& value)
    -> utilxx::Opt<EntryValue>;

auto entryValueToJson(EntryValue value)
    -> Json::Value;

auto entryToJson(Entry value)
    -> Json::Value;

} // namespace forge::core
