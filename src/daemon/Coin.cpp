#include <daemon/Coin.hpp>


auto buddy::daemon::getMaturity(Coin c)
    -> std::size_t
{
    switch(c) {
    case Coin::Odin:
        return 50;
    }
}

auto buddy::daemon::getStartingBlock(Coin c)
    -> std::size_t
{
    switch(c) {
    default:
        return 0;
    }
}
