#include <core/Operation.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/ReadWriteDaemonBase.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::ReadWriteDaemonBase;



auto ReadWriteDaemonBase::writeTxToBlockchain(std::string&& txid_input,
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
