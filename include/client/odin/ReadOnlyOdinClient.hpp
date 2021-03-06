#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <client/ReadOnlyClientBase.hpp>
#include <jsonrpccpp/client.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <utils/Opt.hpp>
#include <utils/Result.hpp>

namespace forge::client {

class ReadOnlyOdinClient : public ReadOnlyClientBase
{
public:
    ReadOnlyOdinClient(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       std::int64_t port,
                       core::Coin coin);

    virtual ~ReadOnlyOdinClient() = default;

    auto getNewestBlock() const
        -> utils::Result<core::Block, ClientError> override;

    auto getTransaction(std::string txid) const
        -> utils::Result<core::Transaction, ClientError> override;

    auto resolveTxIn(core::TxIn vin) const
        -> utils::Result<core::TxOut, ClientError> override;

    auto getBlockCount() const
        -> utils::Result<std::int64_t, ClientError> override;

    auto getBlockHash(std::int64_t index) const
        -> utils::Result<std::string, ClientError> override;

    auto getBlock(std::string hash) const
        -> utils::Result<core::Block, ClientError> override;

    auto getOutputValue(std::string txid,
                        std::int64_t index) const
        -> utils::Result<std::int64_t, ClientError> override;

    auto getUnspent() const
        -> utils::Result<std::vector<core::Unspent>,
                          ClientError> override;

    auto getAddresses() const
        -> utils::Result<std::vector<std::string>,
                          ClientError> override;

    auto isMainnet() const
        -> utils::Result<bool,
                          ClientError> override;

protected:
    auto sendcommand(const std::string& command,
                     Json::Value params) const
        -> utils::Result<Json::Value, ClientError>;


private:
    jsonrpc::HttpClient http_client_;
    mutable jsonrpc::Client client_;
};

namespace odin {

auto processGetTransactionResponse(Json::Value&& response,
                                   const Json::Value& params)
    -> utils::Result<core::Transaction, ClientError>;

auto processGetBlockCountResponse(Json::Value&& response,
                                  const Json::Value& params)
    -> utils::Result<std::int64_t, ClientError>;

auto processGetBlockHashResponse(Json::Value&& response,
                                 const Json::Value& params)
    -> utils::Result<std::string, ClientError>;

auto processGetBlockResponse(Json::Value&& response,
                             const Json::Value& params)
    -> utils::Result<core::Block, ClientError>;

auto processGetUnspentResponse(Json::Value&& response,
                               const Json::Value& params)
    -> utils::Result<std::vector<core::Unspent>,
                      ClientError>;

auto processGetAddressesResponse(Json::Value&& response)
    -> utils::Result<std::vector<std::string>,
                      ClientError>;
} // namespace odin

} // namespace forge::client
