
#pragma once

#include <array>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

using EntryKey = std::vector<std::byte>;


using IPv4Value = std::array<std::byte, 4>;
using IPv6Value = std::array<std::byte, 16>;

constexpr std::byte IPv4_VALUE_FLAG = static_cast<std::byte>(0b00000001);
constexpr std::byte IPv6_VALUE_FLAG = static_cast<std::byte>(0b00000010);

using EntryValue = std::variant<IPv4Value,
                                IPv6Value>;

using Entry = std::pair<EntryKey,
                        EntryValue>;
} // namespace buddy::core
