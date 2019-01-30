#include <core/Coin.hpp>
#include <cstdint>

using forge::core::Coin;


auto forge::core::fromString(std::string_view str)
    -> utilxx::Opt<Coin>
{
    if(str == "odin") {
        return Coin::Odin;
    }

    return std::nullopt;
}

auto forge::core::getMaturity(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 20;
    }
}

auto forge::core::getStartingBlock(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 145000;
    default:
        return 0;
    }
}

auto forge::core::getBlockTimeInSeconds(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 60;
    }
}

auto forge::core::getDefaultTxFee(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 50000;
    }
}


auto forge::core::getValidityLength(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 1051200; //1051200 blocks = 2 years
    }
}


auto forge::core::getMinimumTxAmount(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 10000; //1051200 blocks = 2 years
    }
}
