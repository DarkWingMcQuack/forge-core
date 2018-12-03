#include <core/Operation.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <daemon/odin/ReadWriteOdinDaemon.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::WriteOnlyDaemonBase;
using buddy::daemon::ReadWriteOdinDaemon;



auto WriteOnlyDaemonBase::writeTxToBlockchain(std::string&& txid_input,
                                              std::size_t index,
                                              std::vector<std::byte>&& metadata,
                                              std::size_t burn_value,
                                              std::vector<
                                                  std::pair<std::string,
                                                            std::size_t>>&& outputs) const
    -> utilxx::Result<void, DaemonError>
{
    return generateRawTx(std::move(txid_input),
                         index,
                         std::move(metadata),
                         burn_value,
                         std::move(outputs))
        .flatMap([this](auto&& raw_tx) {
            return signRawTx(std::move(raw_tx));
        })
        .flatMap([this](auto&& signed_tx) {
            return sendRawTx(std::move(signed_tx));
        });
}


auto buddy::daemon::make_writing_daemon(const std::string& host,
                                        const std::string& user,
                                        const std::string& password,
                                        std::size_t port,
                                        core::Coin coin)
    -> std::unique_ptr<WriteOnlyDaemonBase>
{
    switch(coin) {
    case core::Coin::Odin:
        return std::make_unique<ReadWriteOdinDaemon>(host,
                                                     user,
                                                     password,
                                                     port,
                                                     coin);
    }
}