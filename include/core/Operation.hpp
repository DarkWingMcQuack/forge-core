#pragma once

#include <array>
#include <cstddef>
#include <vector>


namespace buddy::core {

constexpr static std::array<std::byte, 3> BUDDY_IDENTIFIER_MASK{(std::byte)0xC6,
                                                                (std::byte)0xDC,
                                                                (std::byte)0x75};

auto startsWithBUDDYIdentifier(const std::vector<std::byte>& data)
    -> bool;

} // namespace buddy::core
