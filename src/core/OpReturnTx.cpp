#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

using buddy::core::OpReturnTx;


OpReturnTx::OpReturnTx(std::vector<std::string>&& send_addresses,
                       std::vector<std::string>&& to_addresses,
                       std::size_t op_return_output_value,
                       OpReturnTx::Metadata&& metadata,
                       std::string&& txid)
    : send_addresses_(std::move(send_addresses)),
      to_addresses_(std::move(to_addresses)),
      op_return_output_value_(std::move(op_return_output_value)),
      metadata_(std::move(metadata)),
      txid_(std::move(txid)) {}


auto OpReturnTx::getFromAddresses() const
    -> const std::vector<std::string>&
{
    return send_addresses_;
}

auto OpReturnTx::getFromAddresses()
    -> std::vector<std::string>&
{
    return send_addresses_;
}

auto OpReturnTx::getToAddresses() const
    -> const std::vector<std::string>&
{
    return to_addresses_;
}

auto OpReturnTx::getToAddresses()
    -> std::vector<std::string>&
{
    return to_addresses_;
}

auto OpReturnTx::getOpReturnOutputValue() const
    -> const std::size_t&
{
    return op_return_output_value_;
}

auto OpReturnTx::getOpReturnOutputValue()
    -> std::size_t&
{
    return op_return_output_value_;
}

auto OpReturnTx::getMetadata() const
    -> const Metadata&
{
    return metadata_;
}

auto OpReturnTx::getMetadata()
    -> Metadata&
{
    return metadata_;
}

auto OpReturnTx::getTxid() const
    -> const std::string&
{
    return txid_;
}

auto OpReturnTx::getTxid()
    -> std::string&
{
    return txid_;
}


auto OpReturnTx::getBlock() const
    -> std::size_t
{
    return block_;
}

auto OpReturnTx::getNumberOfSenders() const
    -> std::size_t
{
    return send_addresses_.size();
}

auto OpReturnTx::getNumberOfRecipiens() const
    -> std::size_t
{
    return to_addresses_.size();
}

auto OpReturnTx::hasNonZeroOpReturnValue() const
    -> bool
{
    return op_return_output_value_ > 0;
}
