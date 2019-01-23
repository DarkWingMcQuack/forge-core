#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::daemon {


class ReadOnlyOdinDaemon : public ReadOnlyDaemonBase
{
public:
    ReadOnlyOdinDaemon(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       std::int64_t port,
                       core::Coin coin);

    virtual ~ReadOnlyOdinDaemon() = default;

    auto getNewestBlock() const
        -> utilxx::Result<core::Block, DaemonError> override;

    auto getTransaction(std::string&& txid) const
        -> utilxx::Result<core::Transaction, DaemonError> override;

    auto resolveTxIn(core::TxIn&& vin) const
        -> utilxx::Result<core::TxOut, DaemonError> override;

    auto getBlockCount() const
        -> utilxx::Result<std::int64_t, DaemonError> override;

    auto getBlockHash(std::int64_t index) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto getBlock(std::string&& hash) const
        -> utilxx::Result<core::Block, DaemonError> override;

    auto getOutputValue(std::string txid,
                        std::int64_t index) const
        -> utilxx::Result<std::int64_t, DaemonError> override;

    auto getUnspent() const
        -> utilxx::Result<std::vector<core::Unspent>,
                          DaemonError> override;

    auto getAddresses() const
        -> utilxx::Result<std::vector<std::string>,
                          DaemonError> override;

protected:
    auto sendcommand(const std::string& command,
                     Json::Value params) const
        -> utilxx::Result<Json::Value, DaemonError>;


private:
    jsonrpc::HttpClient http_client_;
    mutable jsonrpc::Client client_;
};

namespace odin {

auto processGetTransactionResponse(Json::Value&& response,
                                   const Json::Value& params)
    -> utilxx::Result<core::Transaction, DaemonError>;

auto processGetBlockCountResponse(Json::Value&& response,
                                  const Json::Value& params)
    -> utilxx::Result<std::int64_t, DaemonError>;

auto processGetBlockHashResponse(Json::Value&& response,
                                 const Json::Value& params)
    -> utilxx::Result<std::string, DaemonError>;

auto processGetBlockResponse(Json::Value&& response,
                             const Json::Value& params)
    -> utilxx::Result<core::Block, DaemonError>;

auto processGetUnspentResponse(Json::Value&& response,
                               const Json::Value& params)
    -> utilxx::Result<std::vector<core::Unspent>,
                      DaemonError>;

auto processGetAddressesResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::string>,
                      DaemonError>;
} // namespace odin

} // namespace forge::daemon
