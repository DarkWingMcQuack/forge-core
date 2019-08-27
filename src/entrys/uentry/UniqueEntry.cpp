#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <utilxx/Overload.hpp>
#include <variant>
#include <vector>

using forge::core::UniqueEntry;
using forge::core::NoneValue;
using forge::core::ByteArray;
using forge::core::UniqueEntryValue;
using forge::core::EntryKey;
using forge::core::ENTRY_VALUE_FLAG_INDEX;
using forge::core::NONE_VALUE_FLAG;
using forge::core::BYTE_ARRAY_VALUE_FLAG;
using forge::core::IPv4_VALUE_FLAG;
using forge::core::IPv6_VALUE_FLAG;
using forge::core::IPv4Value;
using forge::core::IPv6Value;


auto forge::core::parseUniqueValue(const std::vector<std::byte>& data)
    -> utilxx::Opt<UniqueEntryValue>
{
    return parseUMValue(data)
        .map([](auto&& um_value) {
            return std::visit(
                [](auto&& value) {
                    return UniqueEntryValue{std::move(value)};
                },
                std::move(um_value));
        });
}

auto forge::core::parseUniqueKey(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryKey>
{
    return parseUMKey(data);
}

auto forge::core::parseUniqueEntry(const std::vector<std::byte>& data)
    -> utilxx::Opt<UniqueEntry>
{
    //3 bytes mask
    //1 tokeyn type flag
    //1 op flag
    //1 value flag
    //at least 4 value
    //at least 1 key
    if(data.size() < 11) {
        return std::nullopt;
    }

    //check that the metadata actualy refers to a mutable entry
    if(data[TOKEN_TYPE_INDEX] != UNIQUE_ENTRY_IDENTIFICATION_FLAG) {
        return std::nullopt;
    }

    auto value_opt = parseUniqueValue(data);
    auto key_opt = parseUniqueKey(data);


    return combine(std::move(key_opt),
                   std::move(value_opt));
}

auto forge::core::extractUniqueValueFlag(const UniqueEntryValue& value)
    -> std::byte
{
    static constexpr auto value_flag_visitor =
        utilxx::overload{
            [](const IPv4Value&) { return static_cast<std::byte>(0b00000001); },
            [](const IPv6Value&) { return static_cast<std::byte>(0b00000010); },
            [](const NoneValue&) { return static_cast<std::byte>(0b00000100); },
            [](const ByteArray&) { return static_cast<std::byte>(0b00001000); }};

    return std::visit(value_flag_visitor, value);
}

auto forge::core::uniqueEntryValueToRawData(const UniqueEntryValue& value)
    -> std::vector<std::byte>
{
    return umEntryValueToRawData(value);
}

auto forge::core::uniqueEntryToRawData(const UniqueEntry& entry)
    -> std::vector<std::byte>
{
    auto key_data = std::move(entry.first);
    auto value_data = uniqueEntryValueToRawData(entry.second);
    auto value_flag = extractValueFlag(entry.second);

    value_data.insert(std::begin(value_data),
                      value_flag);

    value_data.insert(std::end(value_data),
                      std::begin(key_data),
                      std::end(key_data));

    return value_data;
}

auto forge::core::jsonToUniqueEntryValue(Json::Value&& value)
    -> utilxx::Opt<UniqueEntryValue>
{
    return jsonToUMEntryValue(std::move(value))
        .map([](auto&& um_value) {
            return std::visit(
                [](auto&& entry_value) {
                    return UniqueEntryValue{std::move(entry_value)};
                },
                std::move(um_value));
        });
}

auto forge::core::uniqueEntryValueToJson(UniqueEntryValue value)
    -> Json::Value
{
    return umentryValueToJson(std::move(value));
}

auto forge::core::uniqueEntryToJson(UniqueEntry value)
    -> Json::Value

{
    Json::Value ret_json;
    ret_json["entry_type"] = "unique entry";
    ret_json["key"] = forge::core::toHexString(value.first);

    auto trash_json = forge::core::uniqueEntryValueToJson(value.second);

    ret_json["type"] = std::move(trash_json["type"]);
    ret_json["value"] = std::move(trash_json["value"]);

    return ret_json;
}
