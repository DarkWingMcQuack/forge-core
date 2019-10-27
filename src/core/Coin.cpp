#include <core/Coin.hpp>
#include <cstdint>
#include <g3log/g3log.hpp>

using forge::core::Coin;


auto forge::core::fromString(std::string_view str)
    -> utilxx::Opt<Coin>
{
    if(str == "odin") {
        return Coin::Odin;
    }

    if(str == "todin") {
        return Coin::tOdin;
    }

    return std::nullopt;
}

auto forge::core::getMaturity(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 20;
    case Coin::tOdin:
        return 10;
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}

auto forge::core::getStartingBlock(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
        return 145000;
    case Coin::tOdin:
        return 10;
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}

auto forge::core::getBlockTimeInSeconds(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
    case Coin::tOdin:
        return 60;
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}

auto forge::core::getDefaultTxFee(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
    case Coin::tOdin:
        return 50000;
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}


auto forge::core::getValidityLength(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
    case Coin::tOdin:
        return 1051200; //1051200 blocks = 2 years
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}


auto forge::core::getMinimumTxAmount(Coin c)
    -> std::int64_t
{
    switch(c) {
    case Coin::Odin:
    case Coin::tOdin:
        return 10000;
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return 0;
    }
}
