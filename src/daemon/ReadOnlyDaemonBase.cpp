#include <core/Coin.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <memory>

using buddy::daemon::ReadOnlyDaemonBase;
using buddy::daemon::ReadOnlyOdinDaemon;

auto ReadOnlyDaemonBase::getCoin() const
    -> core::Coin
{
    return coin_;
}

auto buddy::daemon::make_readonly_daemon(const std::string& host,
                                         const std::string& user,
                                         const std::string& password,
                                         std::size_t port,
                                         core::Coin coin)
    -> std::unique_ptr<ReadOnlyDaemonBase>
{
    switch(coin) {
    case core::Coin::Odin:
        return std::make_unique<ReadOnlyOdinDaemon>(host,
                                                    user,
                                                    password,
                                                    port,
                                                    coin);
    }
}
