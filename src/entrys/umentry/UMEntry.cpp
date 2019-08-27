#include <core/FlagIndexes.hpp>
#include <cstddef>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <g3log/g3log.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <vector>


using utilxx::Opt;
using forge::core::UMEntry;
using forge::core::IPv4Value;
using forge::core::IPv6Value;
using forge::core::ByteArray;
using forge::core::NoneValue;
using forge::core::ENTRY_VALUE_FLAG_INDEX;
using forge::core::IPv4_VALUE_FLAG;
using forge::core::IPv6_VALUE_FLAG;
using forge::core::NONE_VALUE_FLAG;
using forge::core::BYTE_ARRAY_VALUE_FLAG;


auto forge::core::parseValue(const std::vector<std::byte>& data)
    -> utilxx::Opt<UMEntryValue>
{
    if(data[ENTRY_VALUE_FLAG_INDEX] == NONE_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 1) {
        return UMEntryValue{NoneValue{}};
    }

    if(data[ENTRY_VALUE_FLAG_INDEX] == BYTE_ARRAY_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 2) {
        auto value_length = static_cast<std::int64_t>(data[5]);

        //check the bounds
        if(ENTRY_VALUE_FLAG_INDEX + 2 + value_length > data.size()) {
            return std::nullopt;
        }

        ByteArray byte_array;
        std::copy(std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 2,
                  std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 2 + value_length,
                  std::begin(byte_array));

        return UMEntryValue{std::move(byte_array)};
    }

    if(data[ENTRY_VALUE_FLAG_INDEX] == IPv4_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 5) {
        IPv4Value ipv4;
        std::copy(std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 1,
                  std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 5,
                  std::begin(ipv4));
        return UMEntryValue{std::move(ipv4)};
    }


    if(data[ENTRY_VALUE_FLAG_INDEX] == IPv6_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 17) {

        IPv6Value ipv6;
        std::copy(std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 1,
                  std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 17,
                  std::begin(ipv6));

        return UMEntryValue{std::move(ipv6)};
    }

    return std::nullopt;
}

auto forge::core::parseKey(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryKey>
{
    if(data[ENTRY_VALUE_FLAG_INDEX] == NONE_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 1) {

        return EntryKey{std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 1,
                        std::end(data)};
    }

    if(data[ENTRY_VALUE_FLAG_INDEX] == BYTE_ARRAY_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 2) {
        auto value_length = static_cast<std::int64_t>(data[ENTRY_VALUE_FLAG_INDEX + 1]);

        //check the bounds
        if(ENTRY_VALUE_FLAG_INDEX + 2 + value_length > data.size()) {
            return std::nullopt;
        }

        return EntryKey{std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 2 + value_length,
                        std::end(data)};
    }

    if(data[ENTRY_VALUE_FLAG_INDEX] == IPv4_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 5) {

        return EntryKey{std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 5,
                        std::end(data)};
    }



    if(data[ENTRY_VALUE_FLAG_INDEX] == IPv6_VALUE_FLAG
       && data.size() > ENTRY_VALUE_FLAG_INDEX + 17) {

        return EntryKey{std::begin(data) + ENTRY_VALUE_FLAG_INDEX + 17,
                        std::end(data)};
    }

    return std::nullopt;
}

auto forge::core::parseUMEntry(const std::vector<std::byte>& data)
    -> Opt<UMEntry>
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
    if(data[TOKEN_TYPE_INDEX] != UMENTRY_IDENTIFICATION_FLAG) {
        return std::nullopt;
    }

    auto value_opt = parseValue(data);
    auto key_opt = parseKey(data);


    return combine(std::move(key_opt),
                   std::move(value_opt));
}

auto forge::core::extractValueFlag(const UMEntryValue& value)
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


auto forge::core::umEntryValueToRawData(const UMEntryValue& value)
    -> std::vector<std::byte>
{

    static constexpr auto data_extract_visitor =
        utilxx::overload{
            [](const IPv4Value& value) {
                return std::vector<std::byte>(std::begin(value),
                                              std::end(value));
            },
            [](const IPv6Value& value) {
                return std::vector<std::byte>(std::begin(value),
                                              std::end(value));
            },
            [](const NoneValue& value) {
                return std::vector<std::byte>{};
            },
            [](const ByteArray& value) {
                return value;
            }};

    return std::visit(data_extract_visitor, value);
}


auto forge::core::umEntryToRawData(const UMEntry& entry)
    -> std::vector<std::byte>
{
    auto key_data = std::move(entry.first);
    auto value_data = umEntryValueToRawData(entry.second);
    auto value_flag = extractValueFlag(entry.second);

    value_data.insert(std::begin(value_data),
                      value_flag);

    value_data.insert(std::end(value_data),
                      std::begin(key_data),
                      std::end(key_data));

    return value_data;
}


//TODO: test
auto forge::core::jsonToUMEntryValue(Json::Value&& value)
    -> utilxx::Opt<UMEntryValue>
{
    if(value.isNull()) {
        return UMEntryValue{NoneValue{}};
    }

    if(!value.isMember("type")
       || !value.isMember("value")) {
        return std::nullopt;
    }

    auto type_str = std::move(value["type"]);
    auto value_json = std::move(value["value"]);

    if(type_str == "ipv6") {
        if(!value_json.isString()) {
            return std::nullopt;
        }
        auto value_str = std::move(value_json.asString());
        auto byte_vec_opt = stringToByteVec(value_str);

        if(!byte_vec_opt.hasValue()) {
            return std::nullopt;
        }

        auto byte_vec = std::move(byte_vec_opt.getValue());

        if(byte_vec.size() != 16) {
            return std::nullopt;
        }

        std::array<std::byte, 16> ret_array;
        std::copy_n(byte_vec.begin(),
                    16,
                    ret_array.begin());

        return UMEntryValue{std::move(ret_array)};
    }

    if(type_str == "ipv4") {
        if(!value_json.isString()) {
            return std::nullopt;
        }
        auto value_str = std::move(value_json.asString());
        auto byte_vec_opt = stringToByteVec(value_str);

        if(!byte_vec_opt.hasValue()) {
            return std::nullopt;
        }

        auto byte_vec = std::move(byte_vec_opt.getValue());

        if(byte_vec.size() != 4) {
            return std::nullopt;
        }

        std::array<std::byte, 4> ret_array;
        std::copy_n(byte_vec.begin(),
                    4,
                    ret_array.begin());

        return UMEntryValue{std::move(ret_array)};
    }

    if(type_str == "bytearray") {
        if(!value_json.isString()) {
            return std::nullopt;
        }
        auto value_str = std::move(value_json.asString());
        auto byte_vec_opt = stringToByteVec(value_str);

        if(!byte_vec_opt.hasValue()) {
            return std::nullopt;
        }

        auto byte_vec = std::move(byte_vec_opt.getValue());

        return UMEntryValue{std::move(byte_vec)};
    }

    if(type_str == "none") {
        return UMEntryValue{NoneValue{}};
    }

    return std::nullopt;
}

//TODO: test
auto forge::core::umentryValueToJson(UMEntryValue value)
    -> Json::Value
{
    static const auto visitor =
        utilxx::overload{
            [](IPv4Value&& value) {
                Json::Value json;
                json["type"] = "ipv4";

                std::vector<std::byte> helper(std::begin(value),
                                              std::end(value));

                json["value"] = forge::core::toHexString(helper);

                return json;
            },
            [](IPv6Value&& value) {
                Json::Value json;
                json["type"] = "ipv6";

                std::vector<std::byte> helper(std::begin(value),
                                              std::end(value));

                json["value"] = forge::core::toHexString(helper);

                return json;
            },
            [](ByteArray&& value) {
                Json::Value json;
                json["type"] = "bytearray";
                json["value"] = forge::core::toHexString(value);

                return json;
            },
            [](NoneValue&& value) {
                Json::Value json;
                json["type"] = "none";
                json["value"] = Json::nullValue;

                return json;
            }};

    return std::visit(visitor,
                      std::move(value));
}

auto forge::core::umentryToJson(UMEntry value)
    -> Json::Value
{
    Json::Value ret_json;
    ret_json["entry_type"] = "unique modifiable entry";
    ret_json["key"] = forge::core::toHexString(value.first);

    auto trash_json = forge::core::umentryValueToJson(value.second);

    ret_json["type"] = std::move(trash_json["type"]);
    ret_json["value"] = std::move(trash_json["value"]);

    return ret_json;
}
