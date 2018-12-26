#include <core/Coin.hpp>
#include <cstdint>


auto buddy::core::getMaturity(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 20;
    }
}

auto buddy::core::getStartingBlock(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 58000;
    default:
        return 0;
    }
}

auto buddy::core::getBlockTimeInSeconds(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 60;
    }
}

auto buddy::core::getDefaultTxFee(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 10000;
    default:
        return 9999999999999;
    }
}
