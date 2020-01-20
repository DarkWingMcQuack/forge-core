#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <client/ClientError.hpp>
#include <client/WriteOnlyClientBase.hpp>
#include <client/odin/ReadOnlyOdinClient.hpp>
#include <json/value.h>
#include <utils/Result.hpp>

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
        -> utils::Result<std::vector<std::byte>,
                          ClientError> override;

    auto signRawTx(std::vector<std::byte> tx) const
        -> utils::Result<std::vector<std::byte>,
                          ClientError> override;

    auto sendRawTx(std::vector<std::byte> tx) const
        -> utils::Result<std::string, ClientError> override;

    auto generateNewAddress() const
        -> utils::Result<std::string, ClientError> override;

    auto burnAmount(std::int64_t amount,
                    std::vector<std::byte> metadata) const
        -> utils::Result<std::string, ClientError> override;

    auto burnOutput(std::string txid,
                    std::int64_t index,
                    std::vector<std::byte> metadata) const
        -> utils::Result<std::string, ClientError> override;

    auto decodeTxidOfRawTx(const std::vector<std::byte>& tx) const
        -> utils::Result<std::string, ClientError> override;

    auto sendToAddress(std::int64_t amount,
                       std::string address) const
        -> utils::Result<std::string, ClientError> override;

    auto burnAmount(std::string txid,
                    std::int64_t index,
                    std::int64_t amount,
                    std::vector<std::byte> metadata,
                    std::string change_address) const
        -> utils::Result<std::string, ClientError> override;

    auto getVOutIdxByAmountAndAddress(std::string txid,
                                      std::int64_t amount,
                                      std::string address) const
        -> utils::Result<std::int64_t, ClientError> override;

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
    -> utils::Result<std::vector<std::byte>,
                      ClientError>;

auto processSignRawTxResponse(Json::Value&& response)
    -> utils::Result<std::vector<std::byte>,
                      ClientError>;

auto processGenerateNewAddressResponse(Json::Value&& response)
    -> utils::Result<std::string, ClientError>;

auto processDecodeTxidOfRawTxResponse(Json::Value&& response)
    -> utils::Result<std::string, ClientError>;

auto processSendToAddressResponse(Json::Value&& response,
                                  const std::string& address)
    -> utils::Result<std::string, ClientError>;

auto processGetVOutIdxByAmountAndAddressResponse(Json::Value&& response,
                                                 std::int64_t amount,
                                                 const std::string& address)
    -> utils::Result<std::int64_t, ClientError>;

} // namespace odin
} // namespace forge::client
