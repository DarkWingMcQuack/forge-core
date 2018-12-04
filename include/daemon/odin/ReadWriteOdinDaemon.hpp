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
                       std::size_t index,
                       std::vector<std::byte>&& metadata,
                       std::size_t burn_value,
                       std::vector<
                           std::pair<std::string,
                                     std::size_t>>&& outputs) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> override;

    auto signRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> override;

    auto sendRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<void, DaemonError> override;

    auto generateNewAddress() const
        -> utilxx::Result<std::string, DaemonError> override;

private:
    auto generateRpcParams(std::string&& input_txid,
                           std::size_t index,
                           std::vector<std::byte>&& metadata,
                           std::size_t burn_value,
                           std::vector<
                               std::pair<std::string,
                                         std::size_t>>&& outputs) const
        -> Json::Value;
};

} // namespace buddy::daemon
