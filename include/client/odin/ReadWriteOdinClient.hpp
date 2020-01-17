#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <client/ClientError.hpp>
#include <client/WriteOnlyClientBase.hpp>
#include <client/odin/ReadOnlyOdinClient.hpp>
#include <json/value.h>
#include <utilxx/Result.hpp>

namespace forge::client {

class ReadWriteOdinClient : public ReadOnlyOdinClient,
                            public WriteOnlyClientBase
{
public:
    ReadWriteOdinClient(ReadWriteOdinClient&&) = default;
    ReadWriteOdinClient(const ReadWriteOdinClient&) = delete;

    using ReadOnlyOdinClient::ReadOnlyOdinClient;

    auto operator=(ReadWriteOdinClient &&)
        -> ReadWriteOdinClient& = delete;
    auto operator=(const ReadWriteOdinClient&)
        -> ReadWriteOdinClient& = delete;

    auto generateRawTx(std::string input_txid,
                       std::int64_t index,
                       std::vector<std::byte> metadata,
                       std::int64_t burn_value,
                       std::vector<
                           std::pair<std::string,
                                     std::int64_t>>
                           outputs) const
        -> utilxx::Result<std::vector<std::byte>,
                          ClientError> override;

    auto signRawTx(std::vector<std::byte> tx) const
        -> utilxx::Result<std::vector<std::byte>,
                          ClientError> override;

    auto sendRawTx(std::vector<std::byte> tx) const
        -> utilxx::Result<std::string, ClientError> override;

    auto generateNewAddress() const
        -> utilxx::Result<std::string, ClientError> override;

    auto burnAmount(std::int64_t amount,
                    std::vector<std::byte> metadata) const
        -> utilxx::Result<std::string, ClientError> override;

    auto burnOutput(std::string txid,
                    std::int64_t index,
                    std::vector<std::byte> metadata) const
        -> utilxx::Result<std::string, ClientError> override;

    auto decodeTxidOfRawTx(const std::vector<std::byte>& tx) const
        -> utilxx::Result<std::string, ClientError> override;

    auto sendToAddress(std::int64_t amount,
                       std::string address) const
        -> utilxx::Result<std::string, ClientError> override;

    auto burnAmount(std::string txid,
                    std::int64_t index,
                    std::int64_t amount,
                    std::vector<std::byte> metadata,
                    std::string change_address) const
        -> utilxx::Result<std::string, ClientError> override;

    auto getVOutIdxByAmountAndAddress(std::string txid,
                                      std::int64_t amount,
                                      std::string address) const
        -> utilxx::Result<std::int64_t, ClientError> override;

private:
    auto generateRpcParamsForRawTx(std::string input_txid,
                                   std::int64_t index,
                                   std::vector<std::byte> metadata,
                                   std::int64_t burn_value,
                                   std::vector<
                                       std::pair<std::string,
                                                 std::int64_t>>
                                       outputs) const
        -> Json::Value;
};

namespace odin {

auto processGenerateRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::byte>,
                      ClientError>;

auto processSignRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::byte>,
                      ClientError>;

auto processGenerateNewAddressResponse(Json::Value&& response)
    -> utilxx::Result<std::string, ClientError>;

auto processDecodeTxidOfRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::string, ClientError>;

auto processSendToAddressResponse(Json::Value&& response,
                                  const std::string& address)
    -> utilxx::Result<std::string, ClientError>;

auto processGetVOutIdxByAmountAndAddressResponse(Json::Value&& response,
                                                 std::int64_t amount,
                                                 const std::string& address)
    -> utilxx::Result<std::int64_t, ClientError>;

} // namespace odin
} // namespace forge::client
