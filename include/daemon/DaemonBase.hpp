#pragma once

#include <core/OpReturnTx.hpp>
#include <daemon/Coin.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>

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

    virtual auto getOpReturnTxFromTxid(const std::string& txid) const
        -> util::Result<core::OpReturnTx, DaemonError> = 0;

    auto getCoin() const
        -> Coin;

protected:
    Coin coin_;
};



} // namespace buddy::daemon
