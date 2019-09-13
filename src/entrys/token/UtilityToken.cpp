
#include <array>
#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <cstdint>
#include <entrys/token/UtilityToken.hpp>
#include <iterator>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

using forge::core::UtilityToken;
using forge::core::UTILITY_TOKEN_IDENTIFICATION_FLAG;
using forge::core::UTILITY_TOKEN_ID_START_INDEX;
using forge::core::TOKEN_TYPE_INDEX;

UtilityToken::UtilityToken(EntryKey id,
                           std::uint64_t attached_amount)
    : id_(std::move(id)),
      attached_amount_(attached_amount) {}

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

auto UtilityToken::getAttachedAmount() const
    -> std::uint64_t
{
    return attached_amount_;
}

auto UtilityToken::toRawData() const
    -> std::vector<std::byte>
{
    std::array amount_data{
        static_cast<std::byte>((attached_amount_ & 0xff00000000000000) >> 56),
        static_cast<std::byte>((attached_amount_ & 0x00ff000000000000) >> 48),
        static_cast<std::byte>((attached_amount_ & 0x0000ff0000000000) >> 40),
        static_cast<std::byte>((attached_amount_ & 0x000000ff00000000) >> 32),
        static_cast<std::byte>((attached_amount_ & 0x00000000ff000000) >> 24),
        static_cast<std::byte>((attached_amount_ & 0x0000000000ff0000) >> 16),
        static_cast<std::byte>((attached_amount_ & 0x000000000000ff00) >> 8),
        static_cast<std::byte>(attached_amount_ & 0x000000000000000ff)};

    std::vector ret_vec(std::cbegin(amount_data),
                        std::cend(amount_data));

    std::copy(std::cbegin(id_),
              std::cend(id_),
              std::back_inserter(ret_vec));

    return ret_vec;
}

auto UtilityToken::toJson() const
    -> Json::Value
{
    Json::Value ret_json;
    ret_json["entry_type"] = "utility token";
    ret_json["id"] = toHexString(id_);
    ret_json["amount"] = attached_amount_;

    return ret_json;
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

    std::uint64_t amount =
        (static_cast<std::uint64_t>(metadata[5]) << 56)
        | (static_cast<std::uint64_t>(metadata[6]) << 48)
        | (static_cast<std::uint64_t>(metadata[7]) << 40)
        | (static_cast<std::uint64_t>(metadata[8]) << 32)
        | (static_cast<std::uint64_t>(metadata[9]) << 24)
        | (static_cast<std::uint64_t>(metadata[10]) << 16)
        | (static_cast<std::uint64_t>(metadata[11]) << 8)
        | (static_cast<std::uint64_t>(metadata[12]));

    return UtilityToken{std::move(id),
                        amount};
}
