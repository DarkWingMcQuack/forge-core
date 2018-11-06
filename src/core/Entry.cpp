#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <cstddef>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <vector>


using utilxx::Opt;
using buddy::core::Entry;
using buddy::core::IPv4Value;
using buddy::core::IPv6Value;
using buddy::core::ByteArray;
using buddy::core::NoneValue;
using buddy::core::VALUE_FLAG_INDEX;
using buddy::core::IPv4_VALUE_FLAG;
using buddy::core::IPv6_VALUE_FLAG;
using buddy::core::NONE_VALUE_FLAG;
using buddy::core::BYTE_ARRAY_VALUE_FLAG;


auto buddy::core::parseValue(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryValue>
{
    if(data[VALUE_FLAG_INDEX] == NONE_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 1) {
        return EntryValue{NoneValue{}};
    }

    if(data[VALUE_FLAG_INDEX] == BYTE_ARRAY_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 2) {
        auto value_length = static_cast<std::size_t>(data[5]);

        //check the bounds
        if(VALUE_FLAG_INDEX + 2 + value_length > data.size()) {
            return std::nullopt;
        }

        ByteArray byte_array;
        std::copy(std::begin(data) + VALUE_FLAG_INDEX + 2,
                  std::begin(data) + VALUE_FLAG_INDEX + 2 + value_length,
                  std::begin(byte_array));

        return EntryValue{std::move(byte_array)};
    }

    if(data[VALUE_FLAG_INDEX] == IPv4_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 5) {
        IPv4Value ipv4;
        std::copy(std::begin(data) + VALUE_FLAG_INDEX + 1,
                  std::begin(data) + VALUE_FLAG_INDEX + 5,
                  std::begin(ipv4));
        return EntryValue{std::move(ipv4)};
    }


    if(data[VALUE_FLAG_INDEX] == IPv6_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 17) {

        IPv6Value ipv6;
        std::copy(std::begin(data) + VALUE_FLAG_INDEX + 1,
                  std::begin(data) + VALUE_FLAG_INDEX + 17,
                  std::begin(ipv6));

        return EntryValue{std::move(ipv6)};
    }

    return std::nullopt;
}

auto buddy::core::parseKey(const std::vector<std::byte>& data)
    -> utilxx::Opt<EntryKey>
{
    if(data[VALUE_FLAG_INDEX] == NONE_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 1) {

        return EntryKey{std::begin(data) + VALUE_FLAG_INDEX + 1,
                        std::end(data)};
    }

    if(data[VALUE_FLAG_INDEX] == BYTE_ARRAY_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 2) {
        auto value_length = static_cast<std::size_t>(data[VALUE_FLAG_INDEX + 1]);

        //check the bounds
        if(VALUE_FLAG_INDEX + 2 + value_length > data.size()) {
            return std::nullopt;
        }

        return EntryKey{std::begin(data) + VALUE_FLAG_INDEX + 2 + value_length,
                        std::end(data)};
    }

    if(data[VALUE_FLAG_INDEX] == IPv4_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 5) {

        return EntryKey{std::begin(data) + VALUE_FLAG_INDEX + 5,
                        std::end(data)};
    }



    if(data[VALUE_FLAG_INDEX] == IPv6_VALUE_FLAG
       && data.size() > VALUE_FLAG_INDEX + 17) {

        return EntryKey{std::begin(data) + VALUE_FLAG_INDEX + 17,
                        std::end(data)};
    }

    return std::nullopt;
}

auto buddy::core::parseEntry(const std::vector<std::byte>& data)
    -> Opt<Entry>
{
    //3 bytes mask
    //1 op flag
    //1 value flag
    //at least 4 value
    //at least 1 key
    if(data.size() < 10) {
        return std::nullopt;
    }

    auto value_opt = parseValue(data);
    auto key_opt = parseKey(data);


    return combine(std::move(key_opt),
                   std::move(value_opt));
}

auto buddy::core::extractValueFlag(const EntryValue& value)
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


auto buddy::core::entryValueToRawData(const EntryValue& value)
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


auto buddy::core::entryToRawData(const Entry& entry)
    -> std::vector<std::byte>
{
    auto key_data = std::move(entry.first);
    auto value_data = entryValueToRawData(entry.second);
    auto flag = extractValueFlag(entry.second);

    value_data.insert(std::begin(value_data), flag);

    value_data.insert(std::end(value_data),
                      std::begin(key_data),
                      std::end(key_data));

    return value_data;
}
