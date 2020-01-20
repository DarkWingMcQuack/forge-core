#pragma once

#include <array>
#include <cstddef>
#include <json/value.h>
#include <memory>
#include <utils/Opt.hpp>
#include <vector>

namespace forge::core {

class TxIn
{
public:
    TxIn(std::string&& txid,
         std::int64_t vout_index);

    TxIn(TxIn&&) = default;
    TxIn(const TxIn&) = default;

    auto operator=(TxIn &&)
        -> TxIn& = default;
    auto operator=(const TxIn&)
        -> TxIn& = default;

    auto getTxid() const
        -> const std::string&;
    auto getTxid()
        -> std::string&;

    auto getVoutIndex() const
        -> std::int64_t;

private:
    std::string txid_;
    std::int64_t vout_index_;
};

class TxOut
{
public:
    TxOut(std::int64_t value,
          std::string&& hex,
          std::vector<std::string>&& addresses);

    TxOut(TxOut&&) = default;
    TxOut(const TxOut&) = default;

    auto operator=(TxOut &&)
        -> TxOut& = default;
    auto operator=(const TxOut&)
        -> TxOut& = default;

    auto getValue() const
        -> std::int64_t;

    auto getHex() const
        -> const std::string&;
    auto getHex()
        -> std::string&;

    auto getAddresses() const
        -> const std::vector<std::string>&;
    auto getAddresses()
        -> std::vector<std::string>&;

    auto numberOfAddresses() const
        -> std::int64_t;

    auto isOpReturnOutput() const
        -> bool;

private:
    std::int64_t value_;
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
    Transaction(const Transaction&) = default;

    auto operator=(Transaction &&)
        -> Transaction& = default;
    auto operator=(const Transaction&)
        -> Transaction& = default;

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

    auto getValueOfOutput(std::int64_t index) const
        -> utils::Opt<std::int64_t>;

    auto hasOpReturnOutput() const
        -> bool;

    auto hasExactlyOneOpReturnOutput() const
        -> bool;

    auto getFirstOpReturnOutput() const
        -> utils::Opt<std::reference_wrapper<const TxOut>>;

    auto getFirstOpReturnOutput()
        -> utils::Opt<std::reference_wrapper<TxOut>>;

    auto getFirstNonOpReturnOutput() const
        -> utils::Opt<std::reference_wrapper<const TxOut>>;

    auto getFirstNonOpReturnOutput()
        -> utils::Opt<std::reference_wrapper<TxOut>>;

    auto hasExactlyOneInput() const
        -> bool;

    auto getNumberOfOutputs() const
        -> std::int64_t;

private:
    std::vector<TxIn> inputs_;
    std::vector<TxOut> outputs_;
    std::string txid_;
};

class Unspent
{
public:
    Unspent() = delete;
    Unspent(Unspent&&) = default;
    Unspent(const Unspent&) = default;
    Unspent(std::int64_t value,
            std::int64_t vout_idx,
            std::int64_t confirmations,
            std::string address,
            std::string txid);

    auto operator=(Unspent &&)
        -> Unspent& = default;
    auto operator=(const Unspent&)
        -> Unspent& = default;

    auto getValue() const
        -> std::int64_t;

    auto getVoutIdx() const
        -> std::int64_t;

    auto getConfiramtions() const
        -> std::int64_t;

    auto getTxid() const
        -> const std::string&;
    auto getTxid()
        -> std::string&;

    auto getAddress() const
        -> const std::string&;
    auto getAddress()
        -> std::string&;

private:
    std::int64_t value_;
    std::int64_t vout_idx_;
    std::int64_t confirmations_;
    std::string address_;
    std::string txid_;
};

auto buildTxIn(Json::Value&& json)
    -> utils::Opt<TxIn>;
auto buildTxOut(Json::Value&& json)
    -> utils::Opt<TxOut>;
auto buildTransaction(Json::Value&& json)
    -> utils::Opt<Transaction>;

auto extractMetadata(std::string&& hex)
    -> utils::Opt<std::vector<std::byte>>;

auto stringToByteVec(const std::string& str)
    -> utils::Opt<std::vector<std::byte>>;

auto stringToASCIIByteVec(const std::string& str)
    -> std::vector<std::byte>;

auto metadataStartsWithForgeId(const std::vector<std::byte>& metadata)
    -> bool;

auto toHexString(const std::vector<std::byte>& bytes)
    -> std::string;

} // namespace forge::core
