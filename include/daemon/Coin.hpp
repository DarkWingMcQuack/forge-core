#pragma once

#include <cstddef>

namespace buddy::daemon {

enum class Coin {
    Odin
};

auto getMaturity(Coin c)
    -> std::size_t;

auto getStartingBlock(Coin c)
    -> std::size_t;

} // namespace buddy::daemon
