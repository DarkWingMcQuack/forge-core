#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <json/value.h>
#include <utilxx/Result.hpp>

namespace buddy::daemon {

class ReadWriteOdinDaemon : public ReadOnlyOdinDaemon,
                            public WriteOnlyDaemonBase
{
public:
    ReadWriteOdinDaemon(ReadWriteOdinDaemon&&) = default;
    ReadWriteOdinDaemon(const ReadWriteOdinDaemon&) = delete;

    using ReadOnlyOdinDaemon::ReadOnlyOdinDaemon;

    auto operator=(ReadWriteOdinDaemon &&)
        -> ReadWriteOdinDaemon& = default;
    auto operator=(const ReadWriteOdinDaemon&)
        -> ReadWriteOdinDaemon& = delete;

    auto generateRawTx(std::string&& input_txid,
                       std::int64_t index,
                       std::vector<std::byte>&& metadata,
                       std::int64_t burn_value,
                       std::vector<
                           std::pair<std::string,
                                     std::int64_t>>&& outputs) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> override;

    auto signRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> override;

    auto sendRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto generateNewAddress() const
        -> utilxx::Result<std::string, DaemonError> override;

    auto burnAmount(std::int64_t amount,
                    std::vector<std::byte>&& metadata) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto burnOutput(std::string&& txid,
                    std::int64_t index,
                    std::vector<std::byte>&& metadata) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto decodeTxidOfRawTx(const std::vector<std::byte>& tx) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto sendToAddress(std::int64_t amount,
                       std::string&& address) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto burnAmount(std::string&& txid,
                    std::int64_t index,
                    std::int64_t amount,
                    std::vector<std::byte>&& metadata,
                    std::string&& change_address) const
        -> utilxx::Result<std::string, DaemonError> override;

    auto getVOutIdxByAmountAndAddress(std::string txid,
                                      std::int64_t amount,
                                      std::string address) const
        -> utilxx::Result<std::int64_t, DaemonError> override;

private:
    auto generateRpcParamsForRawTx(std::string&& input_txid,
                                   std::int64_t index,
                                   std::vector<std::byte>&& metadata,
                                   std::int64_t burn_value,
                                   std::vector<
                                       std::pair<std::string,
                                                 std::int64_t>>&& outputs) const
        -> Json::Value;
};

namespace odin {

auto processGenerateRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::byte>,
                      DaemonError>;

auto processSignRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::vector<std::byte>,
                      DaemonError>;

auto processGenerateNewAddressResponse(Json::Value&& response)
    -> utilxx::Result<std::string, DaemonError>;

auto processDecodeTxidOfRawTxResponse(Json::Value&& response)
    -> utilxx::Result<std::string, DaemonError>;

auto processSendToAddressResponse(Json::Value&& response,
                                  const std::string& address)
    -> utilxx::Result<std::string, DaemonError>;

auto processGetVOutIdxByAmountAndAddressResponse(Json::Value&& response,
                                                 std::int64_t amount,
                                                 const std::string& address)
    -> utilxx::Result<std::int64_t, DaemonError>;

} // namespace odin
} // namespace buddy::daemon
