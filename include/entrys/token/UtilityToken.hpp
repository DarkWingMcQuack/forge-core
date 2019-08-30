#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

namespace forge::core {

//flag which identifies an utility token
constexpr static inline auto UTILITY_TOKEN_IDENTIFICATION_FLAG = static_cast<std::byte>(0b00000011);

//need to redefine here to avoid circular header
//dependency
using EntryKey = std::vector<std::byte>;

class UtilityToken
{
public:
    UtilityToken(EntryKey id);

    auto getId() const
        -> const EntryKey&;
    auto getId()
        -> EntryKey&;

    auto operator==(const UtilityToken& rhs) const
        -> bool;
    auto operator!=(const UtilityToken& rhs) const
        -> bool;

private:
    EntryKey id_;
};

auto parseUtilityToken(const std::vector<std::byte>& metadata)
    -> utilxx::Opt<UtilityToken>;


} // namespace forge::core
