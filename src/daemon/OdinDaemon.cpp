#include <core/Block.hpp>
#include <core/OpReturnTx.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <daemon/OdinDaemon.hpp>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::daemon::OdinDaemon;
using buddy::util::Opt;
using buddy::util::Result;
using buddy::core::Block;
using buddy::core::OpReturnTx;

using jsonrpc::Client;
using jsonrpc::JSONRPC_CLIENT_V1;
using jsonrpc::HttpClient;
using jsonrpc::JsonRpcException;


OdinDaemon::OdinDaemon(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       std::size_t port,
                       Coin coin)
    : DaemonBase(coin),
      _http_client("http://"
                   + user
                   + ":"
                   + password
                   + "@"
                   + host
                   + ":"
                   + std::to_string(port)),
      _client(_http_client, JSONRPC_CLIENT_V1) {}
      

