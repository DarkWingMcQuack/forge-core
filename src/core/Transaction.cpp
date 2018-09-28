#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

using buddy::core::Transaction;


Transaction::Transaction(std::vector<std::string>&& send_addresses,
                         std::vector<std::string>&& to_addresses,
                         util::Opt<std::size_t>&& op_return_output_value,
                         util::Opt<Transaction::Metadata>&& metadata,
                         std::string&& txid)
    : send_addresses_(std::move(send_addresses)),
      to_addresses_(std::move(to_addresses)),
      op_return_output_value_(std::move(op_return_output_value)),
      metadata_(std::move(metadata)),
      txid_(std::move(txid)) {}


auto Transaction::getFromAddresses() const
    -> const std::vector<std::string>&
{
    return send_addresses_;
}

auto Transaction::getFromAddresses()
    -> std::vector<std::string>&
{
    return send_addresses_;
}

auto Transaction::getToAddresses() const
    -> const std::vector<std::string>&
{
    return to_addresses_;
}

auto Transaction::getToAddresses()
    -> std::vector<std::string>&
{
    return to_addresses_;
}

auto Transaction::getOpReturnOutputValue() const
    -> const util::Opt<std::size_t>&
{
    return op_return_output_value_;
}

auto Transaction::getOpReturnOutputValue()
    -> util::Opt<std::size_t>&
{
    return op_return_output_value_;
}

auto Transaction::getMetadata() const
    -> const util::Opt<Metadata>&
{
    return metadata_;
}

auto Transaction::getMetadata()
    -> util::Opt<Metadata>&
{
    return metadata_;
}

auto Transaction::getTxid() const
    -> const std::string&
{
    return txid_;
}

auto Transaction::getTxid()
    -> std::string&
{
    return txid_;
}

auto Transaction::getNumberOfSenders() const
    -> std::size_t
{
    return send_addresses_.size();
}

auto Transaction::getNumberOfRecipiens() const
    -> std::size_t
{
    return to_addresses_.size();
}

auto Transaction::hasNonZeroOpReturnValue() const
    -> bool
{
    if(op_return_output_value_) {
        return op_return_output_value_.getValue() > 0;
    }

    return false;
}
