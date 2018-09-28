#pragma once

#include <array>
#include <cstddef>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

constexpr static std::array<std::byte, 3> BUDDY_IDENTIFIER_MASK{static_cast<std::byte>(0xC6),
                                                                static_cast<std::byte>(0xDC),
                                                                static_cast<std::byte>(0x75)};


class Transaction
{
public:
    using Metadata = std::vector<std::byte>;

    Transaction(std::vector<std::string>&& send_addresses,
                std::vector<std::string>&& to_addresses,
                util::Opt<std::size_t>&& op_return_output_value,
                util::Opt<Metadata>&& metadata,
                std::string&& txid);

    Transaction(Transaction&&) = default;
    Transaction(const Transaction&) = delete;

    auto operator=(Transaction&&)
        -> Transaction& = default;
    auto operator=(const Transaction&)
        -> Transaction& = delete;


    auto getFromAddresses() const
        -> const std::vector<std::string>&;
    auto getFromAddresses()
        -> std::vector<std::string>&;

    auto getToAddresses() const
        -> const std::vector<std::string>&;
    auto getToAddresses()
        -> std::vector<std::string>&;

    auto getOpReturnOutputValue() const
        -> const util::Opt<std::size_t>&;
    auto getOpReturnOutputValue()
        -> util::Opt<std::size_t>&;

    auto getMetadata() const
        -> const util::Opt<Metadata>&;
    auto getMetadata()
        -> util::Opt<Metadata>&;

    auto getBlock() const
        -> std::size_t;

    auto getTxid() const
        -> const std::string&;
    auto getTxid()
        -> std::string&;

    auto getNumberOfSenders() const
        -> std::size_t;

    auto getNumberOfRecipiens() const
        -> std::size_t;

    auto hasNonZeroOpReturnValue() const
        -> bool;

private:
    std::vector<std::string> send_addresses_;
    std::vector<std::string> to_addresses_;
    util::Opt<std::size_t> op_return_output_value_;
    util::Opt<Metadata> metadata_;
    std::string txid_;
    std::size_t block_;
};
} // namespace buddy::core
