#include <core/Coin.hpp>
#include <client/ReadOnlyClientBase.hpp>
#include <client/odin/ReadOnlyOdinClient.hpp>
#include <g3log/g3log.hpp>
#include <memory>

using forge::client::ReadOnlyClientBase;
using forge::client::ReadOnlyOdinClient;

auto ReadOnlyClientBase::getCoin() const
    -> core::Coin
{
    return coin_;
}

auto forge::client::make_readonly_client(const std::string& host,
                                         const std::string& user,
                                         const std::string& password,
                                         std::int64_t port,
                                         core::Coin coin)
    -> std::unique_ptr<ReadOnlyClientBase>
{
    switch(coin) {
    case core::Coin::Odin:
    case core::Coin::tOdin:
        return std::make_unique<ReadOnlyOdinClient>(host,
                                                    user,
                                                    password,
                                                    port,
                                                    coin);
    default:
        LOG(FATAL) << "entered default case which should never happen";
        return nullptr;
    }
}
