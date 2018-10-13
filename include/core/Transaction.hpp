#pragma once

#include <array>
#include <cstddef>
// #include <daemon/DaemonBase.hpp>
#include <json/value.h>
#include <memory>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

constexpr static std::array<std::byte, 3> BUDDY_IDENTIFIER_MASK{static_cast<std::byte>(0xC6),
                                                                static_cast<std::byte>(0xDC),
                                                                static_cast<std::byte>(0x75)};

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

    auto getFirstOpReturnOutput() const
        -> util::Opt<std::reference_wrapper<const TxOut>>;

    auto getFirstOpReturnOutput()
        -> util::Opt<std::reference_wrapper<TxOut>>;

    auto getFirstNonOpReturnOutput() const
        -> util::Opt<std::reference_wrapper<const TxOut>>;

    auto getFirstNonOpReturnOutput()
        -> util::Opt<std::reference_wrapper<TxOut>>;

    auto hasExactlyOneInput() const
        -> bool;

    auto getNumberOfOutputs() const
        -> std::size_t;

private:
    std::vector<TxIn> inputs_;
    std::vector<TxOut> outputs_;
    std::string txid_;
};

auto buildTxIn(Json::Value&& json)
    -> util::Opt<TxIn>;
auto buildTxOut(Json::Value&& json)
    -> util::Opt<TxOut>;
auto buildTransaction(Json::Value&& json)
    -> util::Opt<Transaction>;

auto extractMetadata(std::string&& hex)
    -> util::Opt<std::vector<std::byte>>;

auto stringToByteVec(std::string&& str)
    -> util::Opt<std::vector<std::byte>>;

auto metadataStartsWithBuddyId(const std::vector<std::byte>& metadata)
    -> bool;

} // namespace buddy::core
