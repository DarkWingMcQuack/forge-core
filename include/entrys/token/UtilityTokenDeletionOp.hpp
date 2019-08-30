#pragma once


#include <cstdint>
#include <entrys/token/UtilityToken.hpp>

namespace forge::core {

class UtilityTokenDeletionOp
{
public:
    UtilityTokenDeletionOp(UtilityToken token,
                           std::uint64_t amount,
                           std::string creator,
                           std::int64_t block,
                           std::int64_t burn_value);

    auto getUtilityToken() const
        -> const UtilityToken&;
    auto getUtilityToken()
        -> UtilityToken&;

    auto getAmount() const
        -> std::uint64_t;

    auto getBurnValue() const
        -> std::int64_t;

    auto getCreator() const
        -> const std::string&;
    auto getCreator()
        -> std::string&;

    auto getBlock() const
        -> std::int64_t;

private:
    UtilityToken token_;
    std::uint64_t amount_;
    std::string creator_;
    std::int64_t block_;
    std::int64_t burn_value_;
};

auto createUtilityTokenDeletionOpMetadata(UtilityToken&& entry,
                                          std::uint64_t)
    -> std::vector<std::byte>;

} // namespace forge::core
