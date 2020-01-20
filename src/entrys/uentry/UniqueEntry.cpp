#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <utils/Overload.hpp>
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

UniqueEntry::UniqueEntry(EntryKey key, UniqueEntryValue value)
    : key_(std::move(key)),
      value_(std::move(value)) {}

auto UniqueEntry::toRawData() const
    -> std::vector<std::byte>
{
    auto key_data = std::move(getKey());
    auto value_data = uniqueEntryValueToRawData(getValue());
    auto value_flag = extractValueFlag(getValue());

    value_data.insert(std::begin(value_data),
                      value_flag);

    value_data.insert(std::end(value_data),
                      std::begin(key_data),
                      std::end(key_data));

    return value_data;
}

auto UniqueEntry::toJson() const
    -> Json::Value
{
    Json::Value ret_json;
    ret_json["entry_type"] = "unique entry";
    ret_json["key"] = forge::core::toHexString(getKey());

    auto trash_json = forge::core::uniqueEntryValueToJson(getValue());

    ret_json["type"] = std::move(trash_json["type"]);
    ret_json["value"] = std::move(trash_json["value"]);

    return ret_json;
}

auto UniqueEntry::getKey() const
    -> const EntryKey&
{
    return key_;
}

auto UniqueEntry::getKey()
    -> EntryKey&
{
    return key_;
}

auto UniqueEntry::getValue() const
    -> const UniqueEntryValue&
{
    return value_;
}

auto UniqueEntry::getValue()
    -> UniqueEntryValue&
{
    return value_;
}


auto forge::core::parseUniqueValue(const std::vector<std::byte>& data)
    -> utils::Opt<UniqueEntryValue>
{
    return parseUMValue(data)
        .map([](auto um_value) {
            return std::visit(
                [](auto value) {
                    return UniqueEntryValue{std::move(value)};
                },
                std::move(um_value));
        });
}

auto forge::core::parseUniqueKey(const std::vector<std::byte>& data)
    -> utils::Opt<EntryKey>
{
    return parseUMKey(data);
}

auto forge::core::parseUniqueEntry(const std::vector<std::byte>& data)
    -> utils::Opt<UniqueEntry>
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
                   std::move(value_opt))
        .map([](auto pair) {
            return UniqueEntry{std::move(pair.first),
                               std::move(pair.second)};
        });
}

auto forge::core::extractUniqueValueFlag(const UniqueEntryValue& value)
    -> std::byte
{
    static constexpr auto value_flag_visitor =
        utils::overload{
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

auto forge::core::jsonToUniqueEntryValue(Json::Value&& value)
    -> utils::Opt<UniqueEntryValue>
{
    return jsonToUMEntryValue(std::move(value))
        .map([](auto um_value) {
            return std::visit(
                [](auto entry_value) {
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
