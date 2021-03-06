#include <algorithm>
#include <core/FlagIndexes.hpp>
#include <cstdint>
#include <cstring>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <entrys/token/UtilityTokenOwnershipTransferOp.hpp>
#include <iterator>

using forge::core::UtilityToken;
using forge::core::UtilityTokenOwnershipTransferOp;


UtilityTokenOwnershipTransferOp::UtilityTokenOwnershipTransferOp(UtilityToken token,
                                                                 std::uint64_t amount,
                                                                 std::string sender,
                                                                 std::string reciever,
                                                                 std::int64_t block,
                                                                 std::int64_t burn_value)
    : token_(std::move(token)),
      amount_(amount),
      sender_(std::move(sender)),
      reciever_(std::move(reciever)),
      block_(block),
      burn_value_(burn_value) {}


auto UtilityTokenOwnershipTransferOp::getUtilityToken() const
    -> const UtilityToken&
{
    return token_;
}

auto UtilityTokenOwnershipTransferOp::getUtilityToken()
    -> UtilityToken&
{
    return token_;
}

auto UtilityTokenOwnershipTransferOp::getAmount() const
    -> std::uint64_t
{
    return amount_;
}

auto UtilityTokenOwnershipTransferOp::getBurnValue() const
    -> std::int64_t
{
    return burn_value_;
}

auto UtilityTokenOwnershipTransferOp::getCreator() const
    -> const std::string&
{
    return sender_;
}
auto UtilityTokenOwnershipTransferOp::getCreator()
    -> std::string&
{
    return sender_;
}

auto UtilityTokenOwnershipTransferOp::getReciever() const
    -> const std::string&
{
    return reciever_;
}
auto UtilityTokenOwnershipTransferOp::getReciever()
    -> std::string&
{
    return reciever_;
}

auto UtilityTokenOwnershipTransferOp::getBlock() const
    -> std::int64_t
{
    return block_;
}

auto forge::core::createUtilityTokenOwnershipTransferOpMetadata(UtilityToken&& entry)
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
        forge::core::UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG};

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
