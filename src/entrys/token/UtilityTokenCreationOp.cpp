#include <algorithm>
#include <core/FlagIndexes.hpp>
#include <cstdint>
#include <cstring>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <iterator>

using forge::core::UtilityToken;
using forge::core::UtilityTokenCreationOp;


UtilityTokenCreationOp::UtilityTokenCreationOp(UtilityToken token,
                                               std::uint64_t amount,
                                               std::string creator,
                                               std::int64_t block,
                                               std::int64_t burn_value)
    : token_(std::move(token)),
      amount_(amount),
      creator_(std::move(creator)),
      block_(block),
      burn_value_(burn_value) {}


auto UtilityTokenCreationOp::getUtilityToken() const
    -> const UtilityToken&
{
    return token_;
}

auto UtilityTokenCreationOp::getUtilityToken()
    -> UtilityToken&
{
    return token_;
}

auto UtilityTokenCreationOp::getAmount() const
    -> std::uint64_t
{
    return amount_;
}

auto UtilityTokenCreationOp::getBurnValue() const
    -> std::int64_t
{
    return burn_value_;
}

auto UtilityTokenCreationOp::getCreator() const
    -> const std::string&
{
    return creator_;
}
auto UtilityTokenCreationOp::getCreator()
    -> std::string&
{
    return creator_;
}

auto UtilityTokenCreationOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto forge::core::createUtilityTokenCreationOpMetadata(UtilityToken&& entry)
    -> std::vector<std::byte>
{
    auto amount = entry.getAttachedAmount();

    std::array amount_data{
        static_cast<std::byte>((amount & 0xff000000) >> 56),
        static_cast<std::byte>((amount & 0xff000000) >> 48),
        static_cast<std::byte>((amount & 0xff000000) >> 40),
        static_cast<std::byte>((amount & 0xff000000) >> 32),
        static_cast<std::byte>((amount & 0xff000000) >> 24),
        static_cast<std::byte>((amount & 0x00ff0000) >> 16),
        static_cast<std::byte>((amount & 0x0000ff00) >> 8),
        static_cast<std::byte>(amount & 0x000000ff)};

    auto entry_data = std::move(entry.getId());

    std::vector ret_vec{
        forge::core::UTILITY_TOKEN_IDENTIFICATION_FLAG,
        forge::core::UTILITY_TOKEN_CREATION_FLAG};

    std::copy(std::cbegin(amount_data),
              std::cend(amount_data),
              std::back_inserter(ret_vec));

    std::copy(std::cbegin(entry_data),
              std::cend(entry_data),
              std::back_inserter(ret_vec));

    ret_vec.insert(std::begin(ret_vec),
                   std::begin(forge::core::FORGE_IDENTIFIER_MASK),
                   std::end(forge::core::FORGE_IDENTIFIER_MASK));

    return ret_vec;
}
