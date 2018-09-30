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
        -> util::Result<core::Block, std::string> override;

    auto getOpReturnTxFromTxid(const std::string& txid) const
        -> util::Opt<core::OpReturnTx> override;


private:
    jsonrpc::HttpClient _http_client;
    jsonrpc::Client _client;
};

} // namespace buddy::daemon
