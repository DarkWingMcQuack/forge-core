#include <core/Coin.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <g3log/g3log.hpp>
#include <memory>

using forge::daemon::ReadOnlyDaemonBase;
using forge::daemon::ReadOnlyOdinDaemon;

auto ReadOnlyDaemonBase::getCoin() const
    -> core::Coin
{
    return coin_;
}

auto forge::daemon::make_readonly_daemon(const std::string& host,
                                         const std::string& user,
                                         const std::string& password,
                                         std::int64_t port,
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
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return nullptr;
    }
}
