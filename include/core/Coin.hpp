#pragma once

#include <cstddef>

namespace buddy::core {

enum class Coin {
    Odin
};

auto getMaturity(Coin c)
    -> std::size_t;

auto getStartingBlock(Coin c)
    -> std::size_t;

auto getBlockTimeInSeconds(Coin c)
    -> std::size_t;

} // namespace buddy::core
