#pragma once

#include <cstdint>
#include <string_view>
#include <utils/Opt.hpp>

namespace forge::core {

enum class Coin {
    Odin,
    tOdin
};

auto fromString(std::string_view str)
    -> utils::Opt<Coin>;

auto getMaturity(Coin c)
    -> std::int64_t;

auto getStartingBlock(Coin c)
    -> std::int64_t;

auto getBlockTimeInSeconds(Coin c)
    -> std::int64_t;

auto getDefaultTxFee(Coin c)
    -> std::int64_t;

auto getValidityLength(Coin c)
    -> std::int64_t;

auto getMinimumTxAmount(Coin c)
    -> std::int64_t;

} // namespace forge::core
