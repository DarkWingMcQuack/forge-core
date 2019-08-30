
#include "entrys/token/UtilityToken.hpp"
#include "core/FlagIndexes.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

using forge::core::UtilityToken;
using forge::core::UTILITY_TOKEN_IDENTIFICATION_FLAG;
using forge::core::UTILITY_TOKEN_ID_START_INDEX;
using forge::core::TOKEN_TYPE_INDEX;

UtilityToken::UtilityToken(EntryKey id)
    : id_(std::move(id)) {}

auto UtilityToken::getId() const
    -> const EntryKey&
{
    return id_;
}

auto UtilityToken::getId()
    -> EntryKey&
{
    return id_;
}

auto UtilityToken::operator==(const UtilityToken& rhs) const
    -> bool
{
    return id_ == rhs.id_;
}

auto UtilityToken::operator!=(const UtilityToken& rhs) const
    -> bool
{
    return id_ != rhs.id_;
}

auto forge::core::parseUtilityToken(const std::vector<std::byte>& metadata)
    -> utilxx::Opt<UtilityToken>
{
    //3 bytes mask
    //1 tokeyn type flag
    //1 op flag
    //8 amount
    //at least 1 id
    if(metadata.size() < 14) {
        return std::nullopt;
    }

    //check that the metadata actualy refers to a mutable entry
    if(metadata[TOKEN_TYPE_INDEX] != UTILITY_TOKEN_IDENTIFICATION_FLAG) {
        return std::nullopt;
    }

    std::vector id(std::cbegin(metadata) + UTILITY_TOKEN_ID_START_INDEX,
                   std::cend(metadata));

    return UtilityToken{std::move(id)};
}
