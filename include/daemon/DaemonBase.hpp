#pragma once

#include <core/Block.hpp>
#include <core/OpReturnTx.hpp>
#include <core/Transaction.hpp>
#include <daemon/Coin.hpp>
#include <memory>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::daemon {

class DaemonError final : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class DaemonBase
{
public:
    DaemonBase(Coin coin)
        : coin_(coin) {}

    virtual auto getNewestBlock() const
        -> util::Result<core::Block, DaemonError> = 0;

    virtual auto getTransaction(std::string&& txid) const
        -> util::Result<core::Transaction, DaemonError> = 0;

    virtual auto resolveTxIn(core::TxIn&& vin) const
        -> util::Result<core::TxOut, DaemonError> = 0;

    virtual auto getBlockCount() const
        -> util::Result<std::size_t, DaemonError> = 0;

    virtual auto getBlockHash(std::size_t index) const
        -> util::Result<std::string, DaemonError> = 0;

    virtual auto getBlock(std::string&& hash) const
        -> util::Result<core::Block, DaemonError> = 0;

    virtual auto getCoin() const
        -> Coin final;

    virtual ~DaemonBase() = default;

private:
    Coin coin_;
};

auto make_daemon(const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 std::size_t port,
                 Coin coin)
    -> std::unique_ptr<DaemonBase>;



} // namespace buddy::daemon
