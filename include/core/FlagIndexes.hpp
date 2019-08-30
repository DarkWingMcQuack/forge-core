#pragma once

#include <array>
#include <cstddef>

namespace forge::core {

//index of the flag identifying the token type
constexpr static inline auto TOKEN_TYPE_INDEX = 3;

//index of the fag identifying the operation type
constexpr static inline auto OPERATION_FLAG_INDEX = 4;

//index of the flag identifying the type of value for mutable and immutable entry types
constexpr static inline auto ENTRY_VALUE_FLAG_INDEX = 5;

//mask which every forge operation needs to start with to be identified as such
constexpr static inline auto FORGE_IDENTIFIER_MASK =
    std::array{static_cast<std::byte>(0xC6),
               static_cast<std::byte>(0xDC),
               static_cast<std::byte>(0x75)};

} // namespace forge::core
