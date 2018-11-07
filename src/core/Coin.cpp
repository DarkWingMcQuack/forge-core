#include <core/Coin.hpp>


auto buddy::core::getMaturity(Coin c)
    -> std::size_t
{
    switch(c) {
    case Coin::Odin:
        return 50;
    }
}

auto buddy::core::getStartingBlock(Coin c)
    -> std::size_t
{
    switch(c) {
    case Coin::Odin:
        return 49400;
    default:
        return 0;
    }
}

auto buddy::core::getBlockTimeInSeconds(Coin c)
    -> std::size_t
{
    switch(c) {
    case Coin::Odin:
        return 60;
    }
}
