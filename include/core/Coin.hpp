#pragma once

#include <cstdint>
#include <string_view>
#include <utilxx/Opt.hpp>

namespace forge::core {

enum class Coin {
    Odin
};

auto fromString(std::string_view str)
    -> utilxx::Opt<Coin>;

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
