#include <core/Block.hpp>
#include <core/OpReturnTx.hpp>
#include <daemon/Coin.hpp>
#include <daemon/DaemonBase.hpp>
#include <daemon/OdinDaemon.hpp>
#include <fmt/core.h>
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
using buddy::core::buildBlock;
using buddy::core::OpReturnTx;
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
        .map([](auto&& json) {
            return buildBlock(std::move(json));
        })
        .flatMap([&param](auto&& opt)
                     -> util::Result<core::Block, DaemonError> {
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

auto OdinDaemon::resolveTxIn(TxIn&& vin) const
    -> util::Result<TxOut, DaemonError>
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
                     -> util::Result<core::TxOut, DaemonError> {
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

auto OdinDaemon::getTransaction(std::string&& txid) const
    -> util::Result<core::Transaction, DaemonError>
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
                     -> util::Result<core::Transaction, DaemonError> {
            if(opt) {
                return std::move(opt.getValue());
            }

            auto error_str =
                fmt::format("unable to build transaction from result when calling {}, with parameters {}",
                            command,
                            params.asString());

            return DaemonError{std::move(error_str)};
        });
}
