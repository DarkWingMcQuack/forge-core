#pragma once

#include <core/Coin.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <client/ClientError.hpp>
#include <client/ReadOnlyClientBase.hpp>
#include <utils/Result.hpp>

namespace forge::client {

class WriteOnlyClientBase
{
public:
    virtual ~WriteOnlyClientBase() = default;

    //creates, signes and sends a transaction
    virtual auto writeTxToBlockchain(std::string txid_input,
                                     std::int64_t index,
                                     std::vector<std::byte> metadata,
                                     std::int64_t burn_value,
                                     std::vector<
                                         std::pair<std::string,
                                                   std::int64_t>>
                                         outputs) const
        -> utils::Result<std::string, ClientError>;

    //generate a raw transaction with one input
    //on OP_RETURN Output and some other arbitrary outputs
    virtual auto generateRawTx(std::string input_txid,
                               std::int64_t index,
                               std::vector<std::byte> metadata,
                               std::int64_t burn_value,
                               std::vector<
                                   std::pair<std::string,
                                             std::int64_t>>
                                   outputs) const
        -> utils::Result<std::vector<std::byte>,
                          ClientError> = 0;

    //sign a given transaction
    virtual auto signRawTx(std::vector<std::byte> tx) const
        -> utils::Result<std::vector<std::byte>,
                          ClientError> = 0;

    //broadcast a signed tx to the p2p network
    virtual auto sendRawTx(std::vector<std::byte> tx) const
        -> utils::Result<std::string, ClientError> = 0;

    //extracts the txid of a raw transaction
    virtual auto decodeTxidOfRawTx(const std::vector<std::byte>& tx) const
        -> utils::Result<std::string, ClientError> = 0;

    //generate a new address for the wallet
    virtual auto generateNewAddress() const
        -> utils::Result<std::string, ClientError> = 0;

    //sends the given amount of coins to the given address
    virtual auto sendToAddress(std::int64_t amount,
                               std::string address) const
        -> utils::Result<std::string, ClientError> = 0;

    //should check unspent outputs,
    //select an output enought value
    //if value = fee + amount then burn the output completly and write
    //the given metadata to the blockchain
    //if value > amount + fee generate a new address and use this address
    //and send the rest of the output to this address
    virtual auto burnAmount(std::int64_t amount,
                            std::vector<std::byte> metadata) const
        -> utils::Result<std::string, ClientError> = 0;

    //burns the given amount of the given output txid,
    //writes the given metadata to the blockchain
    //and sends the remaining coins of the given output
    //to the change_address
    //requires amount + fee < output value
    virtual auto burnAmount(std::string txid,
                            std::int64_t index,
                            std::int64_t amount,
                            std::vector<std::byte> metadata,
                            std::string change_address) const
        -> utils::Result<std::string, ClientError> = 0;

    //burn a whole output and write the given metadata to the blockchain
    virtual auto burnOutput(std::string txid,
                            std::int64_t index,
                            std::vector<std::byte> metadata) const
        -> utils::Result<std::string, ClientError> = 0;


    virtual auto getVOutIdxByAmountAndAddress(std::string txid,
                                              std::int64_t amount,
                                              std::string address) const
        -> utils::Result<std::int64_t, ClientError> = 0;
};

auto make_writing_client(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         std::int64_t port,
                         core::Coin coin)
    -> std::unique_ptr<WriteOnlyClientBase>;

} // namespace forge::client
