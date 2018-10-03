#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>



using buddy::util::Opt;
using buddy::core::Entry;
using buddy::core::IPv4Value;
using buddy::core::IPv6Value;
using buddy::core::IPv4_VALUE_FLAG;
using buddy::core::IPv6_VALUE_FLAG;


auto buddy::core::parseValue(const std::vector<std::byte>& data)

    -> util::Opt<EntryValue>
{
    if(data.size() < 10) {
        return std::nullopt;
    }

    if(data[4] == static_cast<std::byte>(0b00000001)) {
        IPv4Value ipv4;
        std::copy(std::begin(data) + 5,
                  std::begin(data) + 9,
                  std::begin(ipv4));
        return EntryValue{std::move(ipv4)};
    }


    if(data[4] == static_cast<std::byte>(0b00000010)
       && data.size() > 20) {

        IPv6Value ipv6;
        std::copy(std::begin(data) + 5,
                  std::begin(data) + 21,
                  std::begin(ipv6));

        return EntryValue{std::move(ipv6)};
    }

    return std::nullopt;
}

auto buddy::core::parseKey(const std::vector<std::byte>& data)
    -> util::Opt<EntryKey>

{
    if(data.size() < 10) {
        return std::nullopt;
    }

    if(data[4] == IPv6_VALUE_FLAG) {

        IPv4Value ipv4{data[5],
                       data[6],
                       data[7],
                       data[8]};

        return EntryKey{std::begin(data) + 9,
                        std::end(data)};
    }


    if(data[4] == IPv6_VALUE_FLAG
       && data.size() > 20) {

        return EntryKey{std::begin(data) + 21,
                        std::end(data)};
    }
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
