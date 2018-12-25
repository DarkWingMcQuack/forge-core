#pragma once

#include <core/Coin.hpp>
#include <core/Operation.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <utilxx/Result.hpp>

namespace buddy::daemon {

class WriteOnlyDaemonBase
{
public:
    virtual ~WriteOnlyDaemonBase() = default;

    virtual auto writeTxToBlockchain(std::string&& txid_input,
                                     std::int64_t index,
                                     std::vector<std::byte>&& metadata,
                                     std::int64_t burn_value,
                                     std::vector<
                                         std::pair<std::string,
                                                   std::int64_t>>&& outputs) const
        -> utilxx::Result<void, DaemonError>;

    virtual auto generateRawTx(std::string&& input_txid,
                               std::int64_t index,
                               std::vector<std::byte>&& metadata,
                               std::int64_t burn_value,
                               std::vector<
                                   std::pair<std::string,
                                             std::int64_t>>&& outputs) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> = 0;

    virtual auto signRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError> = 0;

    virtual auto sendRawTx(std::vector<std::byte>&& tx) const
        -> utilxx::Result<void, DaemonError> = 0;

    virtual auto generateNewAddress() const
        -> utilxx::Result<std::string, DaemonError> = 0;

    // virtual auto
};

auto make_writing_daemon(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         std::int64_t port,
                         core::Coin coin)
    -> std::unique_ptr<WriteOnlyDaemonBase>;

} // namespace buddy::daemon
