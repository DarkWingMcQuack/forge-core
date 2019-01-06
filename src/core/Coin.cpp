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
        return 145000;
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
        return 100000;
    }
}


auto buddy::core::getBuddyValidityLength(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 1051200; //1051200 blocks = 2 years
    }
}
