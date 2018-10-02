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
using buddy::core::TxIn;
using buddy::core::TxOut;
using buddy::core::Transaction;

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

auto OdinDaemon::resolveTxIn(TxIn&& vin) const
    -> util::Result<TxOut, DaemonError>
{
    static const auto command = "gettxout"s;

    Json::Value params;
    params.append(std::move(vin.getTxid()));
    params.append(vin.getVoutIndex());

    return sendcommand(command, params)
        .map([](auto&& json) {
            std::vector<std::string> address_vec;

            const auto& json_vec = json["scriptPubKey"]["addresses"];
            auto hex = std::move(json["scriptPubKey"]["hex"].asString());
            auto value = json["value"].asUInt();

            std::transform(std::cbegin(json_vec),
                           std::cend(json_vec),
                           std::back_inserter(address_vec),
                           [](auto&& value) {
                               return std::move(value.asString());
                           });

            return TxOut{value,
                         std::move(hex),
                         std::move(address_vec)};
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
            //move txid out of json
            auto txid = std::move(json["txid"].asString());
            auto vin = std::move(json["vin"]);
            auto vout = std::move(json["vout"]);

            std::vector<TxIn> inputs;
            std::vector<TxOut> outputs;

            //get inputs
            std::transform(std::cbegin(vin),
                           std::cend(vin),
                           std::back_inserter(inputs),
                           [](auto&& input) {
                               auto txid_in = std::move(input["txid"]).asString();
                               auto vout_index = input["vout"].asUInt();

                               return TxIn{std::move(txid_in),
                                           vout_index};
                           });

            //get outputs
            std::transform(std::cbegin(vout),
                           std::cend(vout),
                           std::back_inserter(outputs),
                           [](auto&& output) {
                               auto txid_out = std::move(output["txid"].asString());
                               auto value = output["value"].asUInt();
                               auto addresses_json = std::move(output["addresses"]);

                               std::vector<std::string> addresses;

                               //get all the addresses of an output
                               std::transform(std::cbegin(addresses_json),
                                              std::cend(addresses_json),
                                              std::back_inserter(addresses),
                                              [](auto&& addr) {
                                                  return addr.asString();
                                              });

                               return TxOut{value,
                                            std::move(txid_out),
                                            std::move(addresses)};
                           });

            return Transaction{std::move(inputs),
                               std::move(outputs),
                               std::move(txid)};
        });
}


auto OdinDaemon::getOpReturnTxFromTxid(std::string&& txid) const
    -> Result<Opt<OpReturnTx>, DaemonError>
{
}
