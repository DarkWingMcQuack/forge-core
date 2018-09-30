#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <daemon/OdinDaemon.hpp>
#include <memory>

using buddy::daemon::DaemonBase;
using buddy::daemon::OdinDaemon;
using buddy::daemon::Coin;

auto DaemonBase::getCoin() const
    -> Coin
{
    return coin_;
}

auto buddy::daemon::make_daemon(const std::string& host,
                                const std::string& user,
                                const std::string& password,
                                std::size_t port,
                                Coin coin)
    -> std::unique_ptr<DaemonBase>
{
    switch(coin) {
    case Coin::Odin:
        return std::make_unique<OdinDaemon>(host,
                                            user,
                                            password,
                                            port,
                                            coin);
    }
}
