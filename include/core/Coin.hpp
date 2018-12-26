#pragma once

#include <cstdint>

namespace buddy::core {

enum class Coin {
    Odin
};

auto getMaturity(Coin c)
    -> std::int64_t;

auto getStartingBlock(Coin c)
    -> std::int64_t;

auto getBlockTimeInSeconds(Coin c)
    -> std::int64_t;

auto getDefaultTxFee(Coin c)
    -> std::int64_t;

} // namespace buddy::core
