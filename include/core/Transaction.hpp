#pragma once

#include <array>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

class TxIn
{
public:
    TxIn(std::string&& txid,
         std::size_t vout_index);

    TxIn(TxIn&&) = default;
    TxIn(const TxIn&) = delete;

    auto operator=(TxIn &&)
        -> TxIn& = default;
    auto operator=(const TxIn&)
        -> TxIn& = delete;

    auto getTxid() const
        -> const std::string&;
    auto getTxid()
        -> std::string&;

    auto getVoutIndex() const
        -> std::size_t;

private:
    std::string txid_;
    std::size_t vout_index_;
};

class TxOut
{
public:
    TxOut(std::size_t value,
          std::string&& hex,
          std::vector<std::string>&& addresses);

    TxOut(TxOut&&) = default;
    TxOut(const TxOut&) = default;

    auto operator=(TxOut &&)
        -> TxOut& = default;
    auto operator=(const TxOut&)
        -> TxOut& = delete;

    auto getValue() const
        -> std::size_t;

    auto getHex() const
        -> const std::string&;
    auto getHex()
        -> std::string&;

    auto getAddresses() const
        -> const std::vector<std::string>&;
    auto getAddresses()
        -> std::vector<std::string>&;

    auto numberOfAddresses() const
        -> std::size_t;

    auto isOpReturnOutput() const
        -> bool;

private:
    std::size_t value_;
    std::string hex_;
    std::vector<std::string> addresses_;
};

class Transaction
{
public:
    Transaction(std::vector<TxIn>&& inputs,
                std::vector<TxOut>&& outputs,
                std::string&& txid);

    Transaction(Transaction&&) = default;
    Transaction(const Transaction&) = delete;

    auto operator=(Transaction &&)
        -> Transaction& = default;
    auto operator=(const Transaction&)
        -> Transaction& = delete;

    auto getInputs() const
        -> const std::vector<TxIn>&;
    auto getInputs()
        -> std::vector<TxIn>&;

    auto getOutputs() const
        -> const std::vector<TxOut>&;
    auto getOutputs()
        -> std::vector<TxOut>&;

    auto getTxid() const
        -> const std::string&;
    auto getTxid()
        -> std::string&;

    auto hasOpReturnOutput() const
        -> bool;

    auto hasExactlyOneOpReturnOutput() const
        -> bool;

    auto getOpReturnOutputs() const
        -> std::vector<TxOut>;

    auto hasExactlyOneInput() const
        -> bool;

    auto getNumberOfOutputs() const
        -> std::size_t;

private:
    std::vector<TxIn> inputs_;
    std::vector<TxOut> outputs_;
    std::string txid_;
};

class BUDDYCandidateAddress
{
public:
    BUDDYCandidateAddress(std::vector<std::string>&& input_addresses,
                          std::vector<std::string>&& output_addresses,
                          std::size_t op_return_value,
                          std::string&& metadata);

private:
    std::vector<std::string> input_addresses_;
    std::vector<std::string> output_addresses_;
    std::size_t op_return_value_;
    std::string metadata_;
};

} // namespace buddy::core
