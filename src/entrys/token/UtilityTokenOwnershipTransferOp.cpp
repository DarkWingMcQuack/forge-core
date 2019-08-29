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
                                                                 std::int64_t block)
    : token_(std::move(token)),
      amount_(amount),
      sender_(std::move(sender)),
      reciever_(std::move(reciever)),
      block_(block) {}


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

auto UtilityTokenOwnershipTransferOp::getAmount()
    -> std::uint64_t
{
    return amount_;
}

auto UtilityTokenOwnershipTransferOp::getSender() const
    -> const std::string&
{
    return sender_;
}
auto UtilityTokenOwnershipTransferOp::getSender()
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

namespace {
template<class To, class From>
// clang-format off
typename std::enable_if<
    (sizeof(To) == sizeof(From)) &&
    std::is_trivially_copyable<From>::value &&
    std::is_trivial<To>::value,
    // this implementation requires that To is trivially default constructible
    To>::type
// clang-format on
// constexpr support needs compiler magic
bit_cast(const From& src) noexcept
{
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

} // namespace

auto forge::core::createUtilityTokenOwnershipTransferOpMetadata(UtilityToken&& entry,
                                                                std::uint64_t amount)
    -> std::vector<std::byte>
{
    auto amount_data = bit_cast<std::array<std::byte, 8>>(amount);
    auto entry_data = std::move(entry.getId());

    std::vector ret_vec{
        forge::core::UTILITY_TOKEN_OWNERSHIP_TRANSFER_FLAG,
        forge::core::UTILITY_TOKEN_IDENTIFICATION_FLAG};

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
