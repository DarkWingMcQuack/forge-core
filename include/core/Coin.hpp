#pragma once

#include <cstdint>
#include <string_view>
#include <utilxx/Opt.hpp>

namespace buddy::core {

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

auto getBuddyValidityLength(Coin c)
    -> std::int64_t;

} // namespace buddy::core
