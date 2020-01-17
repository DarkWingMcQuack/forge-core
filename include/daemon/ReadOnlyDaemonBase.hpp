#pragma once

#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/DaemonError.hpp>
#include <memory>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace forge::daemon {

class ReadOnlyDaemonBase
{
public:
    ReadOnlyDaemonBase(core::Coin coin)
        : coin_(coin) {}

    virtual auto getNewestBlock() const
        -> utilxx::Result<core::Block, DaemonError> = 0;

    virtual auto getTransaction(std::string txid) const
        -> utilxx::Result<core::Transaction, DaemonError> = 0;

    virtual auto resolveTxIn(core::TxIn vin) const
        -> utilxx::Result<core::TxOut, DaemonError> = 0;

    virtual auto getBlockCount() const
        -> utilxx::Result<std::int64_t, DaemonError> = 0;

    virtual auto getBlockHash(std::int64_t index) const
        -> utilxx::Result<std::string, DaemonError> = 0;

    virtual auto getBlock(std::string hash) const
        -> utilxx::Result<core::Block, DaemonError> = 0;

    virtual auto getUnspent() const
        -> utilxx::Result<std::vector<core::Unspent>,
                          DaemonError> = 0;

    virtual auto getOutputValue(std::string txid,
                                std::int64_t index) const
        -> utilxx::Result<std::int64_t, DaemonError> = 0;

    virtual auto getAddresses() const
        -> utilxx::Result<std::vector<std::string>,
                          DaemonError> = 0;

    virtual auto isMainnet() const
        -> utilxx::Result<bool, DaemonError> = 0;

    virtual auto getCoin() const
        -> core::Coin final;


    virtual ~ReadOnlyDaemonBase() = default;

private:
    core::Coin coin_;
};

auto make_readonly_daemon(const std::string& host,
                          const std::string& user,
                          const std::string& password,
                          std::int64_t port,
                          core::Coin coin)
    -> std::unique_ptr<ReadOnlyDaemonBase>;



} // namespace forge::daemon
