#pragma once

#include <core/Block.hpp>
#include <core/Transaction.hpp>
#include <daemon/Coin.hpp>
#include <memory>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

namespace buddy::daemon {

class DaemonError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class ReadOnlyDaemonBase
{
public:
    ReadOnlyDaemonBase(Coin coin)
        : coin_(coin) {}

    virtual auto getNewestBlock() const
        -> utilxx::Result<core::Block, DaemonError> = 0;

    virtual auto getTransaction(std::string&& txid) const
        -> utilxx::Result<core::Transaction, DaemonError> = 0;

    virtual auto resolveTxIn(core::TxIn&& vin) const
        -> utilxx::Result<core::TxOut, DaemonError> = 0;

    virtual auto getBlockCount() const
        -> utilxx::Result<std::size_t, DaemonError> = 0;

    virtual auto getBlockHash(std::size_t index) const
        -> utilxx::Result<std::string, DaemonError> = 0;

    virtual auto getBlock(std::string&& hash) const
        -> utilxx::Result<core::Block, DaemonError> = 0;

    virtual auto getCoin() const
        -> Coin final;

    virtual ~ReadOnlyDaemonBase() = default;

private:
    Coin coin_;
};

auto make_daemon(const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 std::size_t port,
                 Coin coin)
    -> std::unique_ptr<ReadOnlyDaemonBase>;



} // namespace buddy::daemon
