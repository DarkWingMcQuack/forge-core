#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadWriteDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <utilxx/Result.hpp>

namespace buddy::daemon {

class ReadWriteOdinDaemon : public ReadOnlyOdinDaemon,
                            public ReadWriteDaemonBase
{
public:
    using ReadOnlyOdinDaemon::ReadOnlyOdinDaemon;

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

private:
    auto getScriptPubKeyOf(std::string&& address) const
        -> utilxx::Result<std::vector<std::byte>,
                          DaemonError>;
};

} // namespace buddy::daemon
