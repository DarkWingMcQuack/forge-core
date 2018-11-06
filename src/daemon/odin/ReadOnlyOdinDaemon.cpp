#include <core/Block.hpp>
#include <daemon/Coin.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <fmt/core.h>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::ReadOnlyOdinDaemon;
using buddy::daemon::DaemonError;
using utilxx::Opt;
using utilxx::Result;
using utilxx::Try;
using buddy::core::Block;
using buddy::core::buildBlock;
using buddy::core::TxIn;
using buddy::core::buildTxIn;
using buddy::core::TxOut;
using buddy::core::buildTxOut;
using buddy::core::Transaction;
using buddy::core::buildTransaction;

using jsonrpc::Client;
using jsonrpc::JSONRPC_CLIENT_V1;
using jsonrpc::HttpClient;
using jsonrpc::JsonRpcException;
using namespace std::string_literals;


ReadOnlyOdinDaemon::ReadOnlyOdinDaemon(const std::string& host,
                                       const std::string& user,
                                       const std::string& password,
                                       std::size_t port,
                                       Coin coin)
    : ReadOnlyDaemonBase(coin),
      http_client_("http://"
                   + user
                   + ":"
                   + password
                   + "@"
                   + host
                   + ":"
                   + std::to_string(port)),
      client_(http_client_, JSONRPC_CLIENT_V1) {}


auto ReadOnlyOdinDaemon::sendcommand(const std::string& command,
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

auto ReadOnlyOdinDaemon::getBlockCount() const
    -> Result<std::size_t, DaemonError>
{
    static const auto command = "getblockcount"s;

    return sendcommand(command, {})
        .flatMap([](auto&& json)
                     -> Result<std::size_t, DaemonError> {
            return json.asUInt();
        });
}

auto ReadOnlyOdinDaemon::getBlockHash(std::size_t index) const
    -> utilxx::Result<std::string, DaemonError>
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

auto ReadOnlyOdinDaemon::getBlock(std::string&& hash) const
    -> utilxx::Result<Block, DaemonError>
{
    static const auto command = "getblock"s;

    //prepare parameters
    Json::Value param;
    param.append(hash);

    //send command
    return sendcommand(command, param)
        .map([](auto&& json) {
            return buildBlock(std::move(json));
        })
        .flatMap([&param](auto&& opt)
                     -> utilxx::Result<core::Block, DaemonError> {
            if(opt) {
                return std::move(opt.getValue());
            }

            auto error_str =
                fmt::format("unable to build transaction from result when calling {}, with parameters {}",
                            command,
                            param.asString());

            return DaemonError{std::move(error_str)};
        });
}

auto ReadOnlyOdinDaemon::getNewestBlock() const
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

auto ReadOnlyOdinDaemon::resolveTxIn(TxIn&& vin) const
    -> utilxx::Result<TxOut, DaemonError>
{
    static const auto command = "gettxout"s;

    Json::Value params;
    params.append(std::move(vin.getTxid()));
    params.append(vin.getVoutIndex());

    return sendcommand(command, params)
        .map([](auto&& json) {
            return buildTxOut(std::move(json));
        })
        .flatMap([&params](auto&& opt)
                     -> utilxx::Result<core::TxOut, DaemonError> {
            if(opt) {
                return opt.getValue();
            }

            auto error_str =
                fmt::format("unable to build transaction from result when calling {}, with parameters {}",
                            command,
                            params.asString());

            return DaemonError{std::move(error_str)};
        });
}

auto ReadOnlyOdinDaemon::getTransaction(std::string&& txid) const
    -> utilxx::Result<core::Transaction, DaemonError>
{
    static const auto command = "getrawtransaction";

    Json::Value params;
    params.append(std::move(txid));
    params.append(1);

    return sendcommand(command, params)
        .map([](auto&& json) {
            return buildTransaction(std::move(json));
        })
        .flatMap([&params](auto&& opt)
                     -> utilxx::Result<core::Transaction, DaemonError> {
            if(opt) {
                return std::move(opt.getValue());
            }

            auto error_str =
                fmt::format("unable to build transaction from result when calling {}, with parameters {}\n",
                            command,
                            params.toStyledString());

            return DaemonError{std::move(error_str)};
        });
}
