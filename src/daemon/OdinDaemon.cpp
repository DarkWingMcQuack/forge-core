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
using buddy::daemon::DaemonError;
using buddy::util::Opt;
using buddy::util::Result;
using buddy::util::Try;
using buddy::core::Block;
using buddy::core::OpReturnTx;

using jsonrpc::Client;
using jsonrpc::JSONRPC_CLIENT_V1;
using jsonrpc::HttpClient;
using jsonrpc::JsonRpcException;
using namespace std::string_literals;


OdinDaemon::OdinDaemon(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       std::size_t port,
                       Coin coin)
    : DaemonBase(coin),
      http_client_("http://"
                   + user
                   + ":"
                   + password
                   + "@"
                   + host
                   + ":"
                   + std::to_string(port)),
      client_(http_client_, JSONRPC_CLIENT_V1) {}


auto OdinDaemon::sendcommand(const std::string& command,
                             const Json::Value& params) const
    -> Result<Json::Value, DaemonError>
{
    return Try<jsonrpc::JsonRpcException>(
               [this](auto&& command,
                      auto&& params) {
                   return client_.CallMethod(command, params);
               },
               command,
               params)
        .mapError([](auto&& error) {
            return DaemonError{error.what()};
        });
}

auto OdinDaemon::getBlockCount() const
    -> Result<std::size_t, DaemonError>
{
    static const auto command = "getblockcount"s;

    return sendcommand(command, {})
        .flatMap([](auto&& json)
                     -> Result<std::size_t, DaemonError> {
            return json.asUInt();
        });
}

auto OdinDaemon::getBlockHash(std::size_t index) const
    -> util::Result<std::string, DaemonError>
{
    static const auto command = "getblockhash"s;

    Json::Value param;
    param.append(index);

    return sendcommand(command, param)
        .map([](auto&& json) {
            //extract blockhash from json
            return json.asString();
        });
}

auto OdinDaemon::getBlock(std::string&& hash) const
    -> util::Result<Block, DaemonError>
{
    static const auto command = "getblock"s;

    //prepare parameters
    Json::Value param;
    param.append(hash);

    //send command
    return sendcommand(command, param)
        .flatMap([&hash](auto&& json)
                     -> util::Result<core::Block, DaemonError> {
            //extract txids from block
            std::vector<std::string> txids;
            std::transform(std::cbegin(json["tx"]),
                           std::cend(json["tx"]),
                           std::back_inserter(txids),
                           [](auto&& json) {
                               return std::move(json.asString());
                           });


            //extract blocktime
            auto time = json["time"].asUInt();
            //extract block height
            auto height = json["height"].asUInt();

            //construct block
            return Block{std::move(txids),
                         height,
                         time,
                         std::move(hash)};
        });
}

auto OdinDaemon::getNewestBlock() const
    -> Result<Block, DaemonError>
{
    return getBlockCount()
        .flatMap([this](auto&& height) {
            return getBlockHash(height)
                .flatMap([this](auto&& hash) {
                    return getBlock(std::move(hash));
                });
        });
}

auto OdinDaemon::getOpReturnTxFromTxid(const std::string& txid) const
    -> util::Result<core::OpReturnTx, DaemonError>
{
}
