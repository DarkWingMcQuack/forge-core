#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

using buddy::core::TxIn;
using buddy::core::TxOut;
using buddy::core::Transaction;
using namespace std::string_literals;


TxIn::TxIn(std::string&& txid,
           std::size_t vout_index)
    : txid_(std::move(txid)),
      vout_index_(vout_index) {}


auto TxIn::getTxid() const
    -> const std::string&
{
    return txid_;
}

auto TxIn::getTxid()
    -> std::string&
{
    return txid_;
}

auto TxIn::getVoutIndex() const
    -> std::size_t
{
    return vout_index_;
}


TxOut::TxOut(std::size_t value,
             std::string&& hex,
             std::vector<std::string>&& addresses)
    : value_(value),
      hex_(std::move(hex)),
      addresses_(std::move(addresses)) {}


auto TxOut::getValue() const
    -> std::size_t
{
    return value_;
}

auto TxOut::getHex() const
    -> const std::string&
{
    return hex_;
}
auto TxOut::getHex()
    -> std::string&
{
    return hex_;
}

auto TxOut::getAddresses() const
    -> const std::vector<std::string>&
{
    return addresses_;
}
auto TxOut::getAddresses()
    -> std::vector<std::string>&
{
    return addresses_;
}

auto TxOut::numberOfAddresses() const
    -> std::size_t
{
    return addresses_.size();
}

auto TxOut::isOpReturnOutput() const
    -> bool
{
    static const auto OP_RETURN_CODE = "6a"s;

    //check if hex starts with OP_RETURN_CODE
    return std::equal(std::cbegin(OP_RETURN_CODE),
                      std::cend(OP_RETURN_CODE),
                      std::begin(hex_));
}


Transaction::Transaction(std::vector<TxIn>&& inputs,
                         std::vector<TxOut>&& outputs,
                         std::string&& txid)
    : inputs_(std::move(inputs)),
      outputs_(std::move(outputs)),
      txid_(std::move(txid)) {}

auto Transaction::getInputs() const
    -> const std::vector<TxIn>&
{
    return inputs_;
}
auto Transaction::getInputs()
    -> std::vector<TxIn>&
{
    return inputs_;
}

auto Transaction::getOutputs() const
    -> const std::vector<TxOut>&
{
    return outputs_;
}
auto Transaction::getOutputs()
    -> std::vector<TxOut>&
{
    return outputs_;
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

auto Transaction::hasOpReturnOutput() const
    -> bool
{
    return std::any_of(std::cbegin(outputs_),
                       std::end(outputs_),
                       [](auto&& out) {
                           return out.isOpReturnOutput();
                       });
}

auto Transaction::hasExactlyOneOpReturnOutput() const
    -> bool
{
    bool found_op_ret{false};
    for(auto&& out : outputs_) {
        if(out.isOpReturnOutput()) {
            if(found_op_ret) {
                return false;
            }

            found_op_ret = true;
        }
    }

    return found_op_ret;
}

auto Transaction::getOpReturnOutputs() const
    -> std::vector<TxOut>
{
    std::vector<TxOut> op_returns;

    std::copy_if(std::cbegin(outputs_),
                 std::cend(outputs_),
                 std::back_inserter(op_returns),
                 [](auto&& out) {
                     return out.isOpReturnOutput();
                 });

    return op_returns;
}

auto Transaction::hasExactlyOneInput() const
    -> bool
{
    return inputs_.size() == 1;
}

auto Transaction::getNumberOfOutputs() const
    -> std::size_t
{
    return outputs_.size();
}
