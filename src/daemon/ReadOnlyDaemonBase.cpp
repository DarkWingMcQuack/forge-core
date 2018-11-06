#include <daemon/Coin.hpp>
#include <daemon/OdinDaemon.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <memory>

using buddy::daemon::ReadOnlyDaemonBase;
using buddy::daemon::OdinDaemon;
using buddy::daemon::Coin;

auto ReadOnlyDaemonBase::getCoin() const
    -> Coin
{
    return coin_;
}

auto buddy::daemon::make_readonly_daemon(const std::string& host,
                                         const std::string& user,
                                         const std::string& password,
                                         std::size_t port,
                                         Coin coin)
    -> std::unique_ptr<ReadOnlyDaemonBase>
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
