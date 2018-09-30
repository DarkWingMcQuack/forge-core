#pragma once

#include <core/OpReturnTx.hpp>
#include <daemon/Coin.hpp>
#include <util/Opt.hpp>
#include <util/Result.hpp>

namespace buddy::daemon {

class DaemonBase
{
public:
    DaemonBase(Coin coin)
        : coin_(coin) {}

    virtual auto getNewestBlock() const
        -> util::Result<core::Block, std::string> = 0;

    virtual auto getOpReturnTxFromTxid(const std::string& txid) const
        -> util::Opt<core::OpReturnTx> = 0;

    auto getCoin() const
        -> Coin;

protected:
    Coin coin_;
};

} // namespace buddy::daemon
