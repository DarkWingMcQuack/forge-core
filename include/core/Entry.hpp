#pragma once

#include <array>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

using EntryKey = std::vector<std::byte>;


// clang-format off
using IPv4Value = std::array<std::byte, 4>;
using IPv6Value = std::array<std::byte, 16>;
class NoneValue{
public:
    auto operator==(const NoneValue&) const
    {
        return true;
    }
};
// clang-format on

constexpr auto IPv4_VALUE_FLAG = static_cast<std::byte>(0b00000001);
constexpr auto IPv6_VALUE_FLAG = static_cast<std::byte>(0b00000010);
constexpr auto NONE_VALUE_FLAG = static_cast<std::byte>(0b00000100);

using EntryValue = std::variant<IPv4Value,
                                IPv6Value,
                                NoneValue>;

using Entry = std::pair<EntryKey,
                        EntryValue>;

auto parseValue(const std::vector<std::byte>& data)
    -> util::Opt<EntryValue>;

auto parseKey(const std::vector<std::byte>& data)
    -> util::Opt<EntryKey>;

auto parseEntry(const std::vector<std::byte>& data)
    -> util::Opt<Entry>;
} // namespace buddy::core
