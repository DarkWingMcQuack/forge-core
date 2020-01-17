#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using forge::daemon::ReadOnlyOdinDaemon;
using forge::daemon::DaemonError;
using utilxx::Opt;
using utilxx::Result;
using utilxx::Try;
using forge::core::getMaturity;
using forge::core::Block;
using forge::core::buildBlock;
using forge::core::TxIn;
using forge::core::buildTxIn;
using forge::core::TxOut;
using forge::core::Unspent;
using forge::core::buildTxOut;
using forge::core::Transaction;
using forge::core::buildTransaction;

using jsonrpc::Client;
using jsonrpc::JSONRPC_CLIENT_V1;
using jsonrpc::HttpClient;
using jsonrpc::JsonRpcException;
using namespace std::string_literals;


ReadOnlyOdinDaemon::ReadOnlyOdinDaemon(const std::string& host,
                                       const std::string& user,
                                       const std::string& password,
                                       std::int64_t port,
                                       core::Coin coin)
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
                                     Json::Value params) const
    -> Result<Json::Value, DaemonError>
{
    return Try<jsonrpc::JsonRpcException>(
               [this](const auto& command,
                      auto params) {
                   return client_.CallMethod(command,
                                             std::move(params));
               },
               command,
               std::move(params))
        .mapError([&](auto error) {
            LOG(WARNING) << command << " failed";
            return DaemonError{error.what()};
        });
}

auto ReadOnlyOdinDaemon::getBlockCount() const
    -> Result<std::int64_t, DaemonError>
{
    static const auto command = "getblockcount"s;

    return sendcommand(command, {})
        .flatMap([](auto json) {
            return odin::processGetBlockCountResponse(std::move(json),
                                                      {});
        });
}

auto ReadOnlyOdinDaemon::getBlockHash(std::int64_t index) const
    -> utilxx::Result<std::string, DaemonError>
{
    static const auto command = "getblockhash"s;

    Json::Value param;
    param.append(index);

    return sendcommand(command, param)
        .flatMap([&](auto json) {
            return odin::processGetBlockHashResponse(std::move(json),
                                                     param);
        });
}

auto ReadOnlyOdinDaemon::getBlock(std::string hash) const
    -> utilxx::Result<Block, DaemonError>
{
    static const auto command = "getblock"s;

    //prepare parameters
    Json::Value param;
    param.append(hash);

    //send command
    return sendcommand(command, param)
        .flatMap([&](auto json) {
            return odin::processGetBlockResponse(std::move(json),
                                                 param);
        });
}

auto ReadOnlyOdinDaemon::getNewestBlock() const
    -> Result<Block, DaemonError>
{
    return getBlockCount()
        .flatMap([this](auto height) {
            return getBlockHash(height);
        })
        .flatMap([this](auto hash) {
            return getBlock(std::move(hash));
        });
}

auto ReadOnlyOdinDaemon::resolveTxIn(TxIn vin) const
    -> utilxx::Result<TxOut, DaemonError>
{
    auto index = vin.getVoutIndex();
    auto txid = std::move(vin.getTxid());

    return getTransaction(std::move(txid))
        .flatMap([&](auto tx)
                     -> utilxx::Result<TxOut, DaemonError> {
            if(static_cast<std::int64_t>(tx.getOutputs().size())
               <= vin.getVoutIndex()) {
                auto what = fmt::format("unable to get output #{} of transaction {}",
                                        index,
                                        tx.getTxid());
                return DaemonError{std::move(what)};
            }

            return tx.getOutputs()[index];
        });
}

auto ReadOnlyOdinDaemon::getTransaction(std::string txid) const
    -> utilxx::Result<core::Transaction, DaemonError>
{
    static const auto command = "getrawtransaction";

    Json::Value params;
    params.append(std::move(txid));
    params.append(1);

    return sendcommand(command, params)
        .flatMap([&](auto json) {
            return odin::processGetTransactionResponse(std::move(json),
                                                       params);
        });
}

auto ReadOnlyOdinDaemon::getUnspent() const
    -> Result<std::vector<Unspent>,
              DaemonError>
{
    static const auto command = "listunspent";

    Json::Value params;
    params.append(getMaturity(getCoin()));
    params.append(99999999);

    return sendcommand(command, params)
        .flatMap([&](auto json) {
            return odin::processGetUnspentResponse(std::move(json),
                                                   params);
        });
}

auto ReadOnlyOdinDaemon::getOutputValue(std::string txid,
                                        std::int64_t index) const
    -> utilxx::Result<std::int64_t, DaemonError>
{
    auto txid_copy = txid;
    return getTransaction(std::move(txid))
        .flatMap([&](auto tx)
                     -> utilxx::Result<std::int64_t, DaemonError> {
            if(auto value_opt = tx.getValueOfOutput(index);
               value_opt) {
                return value_opt.getValue();
            }

            auto error =
                fmt::format("unable to get the output value of output {} of transaction {}",
                            index,
                            txid_copy);

            return DaemonError{std::move(error)};
        });
}

auto ReadOnlyOdinDaemon::getAddresses() const
    -> utilxx::Result<std::vector<std::string>,
                      DaemonError>
{
    static const auto command = "listaddressgroupings";

    return sendcommand(command, {})
        .flatMap([&](auto json) {
            return odin::processGetAddressesResponse(std::move(json));
        });
}


auto ReadOnlyOdinDaemon::isMainnet() const
    -> utilxx::Result<bool,
                      DaemonError>
{
    static const auto command = "getinfo";

    return sendcommand(command, {})
        .flatMap([&](Json::Value json)
                     -> Result<bool, DaemonError> {
            if(!json.isMember("testnet")
               || !json["testnet"].isBool()) {
                return DaemonError{
                    "unable to find \"testnet\" entry "
                    "in the json respond from the \"getinfo\" command"};
            }

            if(!json["testnet"].isBool()) {
                return DaemonError{
                    "\"testnet\" entry in the json "
                    "respond from the \"getinfo\" command is not boolean"};
            }

            return !json["testnet"].asBool();
        });
}

auto forge::daemon::odin::processGetTransactionResponse(Json::Value&& json,
                                                        const Json::Value& params)
    -> utilxx::Result<Transaction, DaemonError>
{
    // auto copy = json;
    if(auto tx_opt = buildTransaction(std::move(json));
       tx_opt) {
        return std::move(tx_opt.getValue());
    }

    auto error_str =
        fmt::format("unable to build transaction from result when calling {}, with parameters {}\n",
                    "getrawtransaction",
                    params.toStyledString());

    return DaemonError{std::move(error_str)};
}

auto forge::daemon::odin::processGetBlockCountResponse(Json::Value&& response,
                                                       const Json::Value & /*params*/)
    -> utilxx::Result<std::int64_t, DaemonError>
{
    if(!response.isInt64()) {
        return DaemonError{"unable to get current block count"};
    }

    return response.asInt64();
}

auto forge::daemon::odin::processGetBlockHashResponse(Json::Value&& response,
                                                      const Json::Value& params)
    -> utilxx::Result<std::string, DaemonError>
{
    if(!response.isString()) {
        auto error = fmt::format("unable to get blockhash with parameters {}",
                                 params.toStyledString());
        return DaemonError{std::move(error)};
    }

    return response.asString();
}

auto forge::daemon::odin::processGetBlockResponse(Json::Value&& response,
                                                  const Json::Value& params)
    -> utilxx::Result<Block, DaemonError>
{
    if(auto block_opt = buildBlock(std::move(response));
       block_opt) {
        return std::move(block_opt.getValue());
    }

    auto error_str =
        fmt::format("unable to build block from result when calling {}, with parameters {}",
                    "getblock",
                    params.asString());


    return DaemonError{std::move(error_str)};
}

auto forge::daemon::odin::processGetUnspentResponse(Json::Value&& response,
                                                    const Json::Value& params)
    -> utilxx::Result<std::vector<Unspent>,
                      DaemonError>
{
    if(!response.isArray()) {
        return DaemonError{"result of \"listunspent\" was not an json array"};
    }

    std::vector<Unspent> ret_vec;

    utilxx::transform_if(
        std::make_move_iterator(std::begin(response)),
        std::make_move_iterator(std::end(response)),
        std::back_inserter(ret_vec),
        [](auto unspent_json) {
            auto value = unspent_json["amount"].asDouble()
                * 100000000.;
            return Unspent{static_cast<std::int64_t>(value),
                           unspent_json["vout"].asInt64(),
                           unspent_json["confirmations"].asInt64(),
                           unspent_json["address"].asString(),
                           unspent_json["txid"].asString()};
        },
        [](auto unspent_json) {
            return unspent_json.isMember("txid")
                && unspent_json.isMember("confirmations")
                && unspent_json.isMember("spendable")
                && unspent_json.isMember("amount")
                && unspent_json.isMember("address")
                && unspent_json.isMember("vout")
                && unspent_json["txid"].isString()
                && unspent_json["confirmations"].isNumeric()
                && unspent_json["spendable"].isBool()
                && unspent_json["amount"].isNumeric()
                && unspent_json["address"].isString()
                && unspent_json["vout"].isNumeric()
                && unspent_json["spendable"].asBool();
        });

    return std::move(ret_vec);
}

auto forge::daemon::odin::processGetAddressesResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::string>,
                      DaemonError>
{
    if(!response.isArray()) {
        return DaemonError{"result of \"listaddressgroupings\" was not an json array"};
    }

    std::vector<std::string> addresses;

    for(auto&& group : response) {
        if(!group.isArray()) {
            continue;
        }

        for(auto&& elem : group) {
            if(!elem.isValidIndex(0)
               || !elem[0].isString()) {
                break;
            }

            addresses.push_back(std::move(elem[0].asString()));
        }
    }

    return addresses;
}
