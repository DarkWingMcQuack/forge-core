#pragma once

#include <core/Block.hpp>
#include <core/OpReturnTx.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::daemon {

class OdinDaemon final : public DaemonBase
{
public:
    OdinDaemon(const std::string& host,
               const std::string& user,
               const std::string& password,
               std::size_t port,
               Coin coin);

    auto getNewestBlock() const
        -> util::Result<core::Block, DaemonError> override;

    auto getOpReturnTxFromTxid(const std::string& txid) const
        -> util::Result<core::OpReturnTx, DaemonError> override;

private:
    auto getBlockCount() const
        -> util::Result<std::size_t, DaemonError>;

    auto getBlockHash(std::size_t index) const
        -> util::Result<std::string, DaemonError>;

    auto getBlock(std::string&& hash) const
        -> util::Result<core::Block, DaemonError>;

    auto sendcommand(const std::string& command,
                     const Json::Value& params) const
        -> util::Result<Json::Value, DaemonError>;


private:
    jsonrpc::HttpClient http_client_;
    mutable jsonrpc::Client client_;
};

} // namespace buddy::daemon
