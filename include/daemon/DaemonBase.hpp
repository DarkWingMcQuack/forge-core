#pragma once

#include <core/OpReturnTx.hpp>
#include <core/Block.hpp>
#include <daemon/Coin.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>
#include <memory>

namespace buddy::daemon {

class DaemonError : public std::runtime_error
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

    virtual auto getOpReturnTxFromTxid(std::string&& txid) const
        -> util::Result<util::Opt<core::OpReturnTx>, DaemonError> = 0;

    auto getCoin() const
        -> Coin;

protected:
    Coin coin_;
};

auto make_daemon(const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 std::size_t port,
                 Coin coin)
    -> std::unique_ptr<DaemonBase>;



} // namespace buddy::daemon
