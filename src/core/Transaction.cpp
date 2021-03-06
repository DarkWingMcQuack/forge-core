#include <array>
#include <core/FlagIndexes.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <client/ReadOnlyClientBase.hpp>
#include <fmt/core.h>
#include <iomanip>
#include <json/value.h>
#include <sstream>
#include <utility>
#include <utils/Opt.hpp>
#include <vector>


using forge::core::TxIn;
using forge::core::TxOut;
using forge::core::Unspent;
using forge::core::Transaction;
using forge::utils::Opt;
using forge::utils::traverse;
using forge::core::FORGE_IDENTIFIER_MASK;
using namespace std::string_literals;


TxIn::TxIn(std::string&& txid,
           std::int64_t vout_index)
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
    -> std::int64_t
{
    return vout_index_;
}


TxOut::TxOut(std::int64_t value,
             std::string&& hex,
             std::vector<std::string>&& addresses)
    : value_(value),
      hex_(std::move(hex)),
      addresses_(std::move(addresses)) {}


auto TxOut::getValue() const
    -> std::int64_t
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
    -> std::int64_t
{
    return addresses_.size();
}

auto TxOut::isOpReturnOutput() const
    -> bool
{
    static const auto op_return_code = "6a"s;

    //check if hex starts with OP_RETURN_CODE
    return std::equal(std::cbegin(op_return_code),
                      std::cend(op_return_code),
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
                       [](const auto& out) {
                           return out.isOpReturnOutput();
                       });
}

auto Transaction::hasExactlyOneOpReturnOutput() const
    -> bool
{
    auto forward_iter = std::find_if(std::cbegin(outputs_),
                                     std::cend(outputs_),
                                     [](const auto& out) {
                                         return out.isOpReturnOutput();
                                     });

    auto backward_iter = std::find_if(std::crbegin(outputs_),
                                      std::crend(outputs_),
                                      [](const auto& out) {
                                          return out.isOpReturnOutput();
                                      });

    return forward_iter == backward_iter.base() - 1
        && forward_iter != std::cend(outputs_);
}

auto Transaction::getFirstOpReturnOutput() const
    -> utils::Opt<std::reference_wrapper<const TxOut>>
{
    using Ret = utils::Opt<std::reference_wrapper<const TxOut>>;

    auto iter = std::find_if(std::cbegin(outputs_),
                             std::cend(outputs_),
                             [](const auto& out) {
                                 return out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::cref(*iter)};
}

auto Transaction::getFirstOpReturnOutput()
    -> utils::Opt<std::reference_wrapper<TxOut>>
{
    using Ret = utils::Opt<std::reference_wrapper<TxOut>>;

    auto iter = std::find_if(std::begin(outputs_),
                             std::end(outputs_),
                             [](const auto& out) {
                                 return out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::ref(*iter)};
}

auto Transaction::getFirstNonOpReturnOutput() const
    -> utils::Opt<std::reference_wrapper<const TxOut>>
{
    using Ret = utils::Opt<std::reference_wrapper<const TxOut>>;

    auto iter = std::find_if(std::cbegin(outputs_),
                             std::cend(outputs_),
                             [](const auto& out) {
                                 return !out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::cref(*iter)};
}

auto Transaction::getFirstNonOpReturnOutput()
    -> utils::Opt<std::reference_wrapper<TxOut>>
{
    using Ret = utils::Opt<std::reference_wrapper<TxOut>>;

    auto iter = std::find_if(std::begin(outputs_),
                             std::end(outputs_),
                             [](const auto& out) {
                                 return !out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::ref(*iter)};
}

auto Transaction::hasExactlyOneInput() const
    -> bool
{
    return inputs_.size() == 1;
}

auto Transaction::getNumberOfOutputs() const
    -> std::int64_t
{
    return outputs_.size();
}

auto Transaction::getValueOfOutput(std::int64_t index) const
    -> utils::Opt<std::int64_t>
{
    if(static_cast<std::int64_t>(outputs_.size()) < index) {
        return std::nullopt;
    }

    return outputs_[index].getValue();
}


Unspent::Unspent(std::int64_t value,
                 std::int64_t vout_idx,
                 std::int64_t confirmations,
                 std::string address,
                 std::string txid)
    : value_(value),
      vout_idx_(vout_idx),
      confirmations_(confirmations),
      address_(std::move(address)),
      txid_(std::move(txid)) {}

auto Unspent::getValue() const
    -> std::int64_t
{
    return value_;
}

auto Unspent::getVoutIdx() const
    -> std::int64_t
{
    return vout_idx_;
}

auto Unspent::getConfiramtions() const
    -> std::int64_t
{
    return confirmations_;
}

auto Unspent::getTxid() const
    -> const std::string&
{
    return txid_;
}

auto Unspent::getTxid()
    -> std::string&
{
    return txid_;
}

auto Unspent::getAddress() const
    -> const std::string&
{
    return address_;
}

auto Unspent::getAddress()
    -> std::string&
{
    return address_;
}

auto forge::core::buildTxIn(Json::Value&& json)
    -> utils::Opt<TxIn>
{
    try {
        if(!json.isMember("txid")
           || !json.isMember("vout")) {
            return std::nullopt;
        }

        auto txid = json["txid"].asString();
        auto vout_index = json["vout"].asUInt();

        return TxIn{std::move(txid),
                    vout_index};
    } catch(...) {
        return std::nullopt;
    }
}

auto forge::core::buildTxOut(Json::Value&& json)
    -> utils::Opt<TxOut>
{
    try {
        //dont check for addresses, since we allow 0 addresses
        //this happens sometimes in nonstandart tx
        if(!json.isMember("scriptPubKey")
           || !json["scriptPubKey"].isMember("hex")
           || !json.isMember("value")) {
            return std::nullopt;
        }


        auto json_vec = std::move(json["scriptPubKey"]["addresses"]);
        auto hex = json["scriptPubKey"]["hex"].asString();
        auto value = json["value"].asDouble();

        std::vector<std::string> addresses;
        std::transform(std::cbegin(json_vec),
                       std::cend(json_vec),
                       std::back_inserter(addresses),
                       [](auto value) {
                           return std::move(value.asString());
                       });

        auto conv_value = static_cast<std::int64_t>(value * 100000000.);

        return TxOut{conv_value,
                     std::move(hex),
                     std::move(addresses)};
    } catch(...) {
        return std::nullopt;
    }
}

auto forge::core::buildTransaction(Json::Value&& json)
    -> utils::Opt<Transaction>
{
    try {
        if(!json.isMember("txid")
           || !json.isMember("vin")
           || !json.isMember("vout")) {
            return std::nullopt;
        }

        auto txid = json["txid"].asString();
        auto vin = std::move(json["vin"]);
        auto vout = std::move(json["vout"]);

        auto inputs =
            traverse(std::vector<Json::Value>(vin.begin(),
                                              vin.end()),
                     [](auto input) {
                         return buildTxIn(std::move(input));
                     })
                .valueOr(std::vector<TxIn>{});

        auto outputs_opt =
            traverse(std::vector<Json::Value>(vout.begin(),
                                              vout.end()),
                     [](auto output) {
                         return buildTxOut(std::move(output));
                     });


        return outputs_opt
            .map([&txid,
                  &inputs](auto outputs) {
                return Transaction{std::move(inputs),
                                   std::move(outputs),
                                   std::move(txid)};
            });
    } catch(...) {
        return std::nullopt;
    }
}

auto forge::core::extractMetadata(std::string&& hex)
    -> utils::Opt<std::vector<std::byte>>
{
    if(hex.size() < 4) {
        return std::nullopt;
    }

    //opcode for op return
    if(hex[0] != '6' || hex[1] != 'a') {
        return std::nullopt;
    }

    //remove the op return Opcode
    //and the next byte
    hex.erase(0, 4);

    return stringToByteVec(hex);
}

auto forge::core::stringToByteVec(const std::string& str)
    -> utils::Opt<std::vector<std::byte>>
{
    //check if the string has even characters
    if(str.length() % 2 != 0) {
        return std::nullopt;
    }

    //check if all characters are [0-1a-fA-F]
    auto is_hex = std::all_of(std::begin(str),
                              std::end(str),
                              [](auto c) {
                                  return std::isxdigit(c);
                              });

    if(!is_hex) {
        return std::nullopt;
    }

    std::vector<std::byte> data;

    for(size_t i = 0; i < str.length(); i += 2) {
        auto byte_string = str.substr(i, 2);
        auto byte = static_cast<std::byte>(std::strtol(byte_string.c_str(), nullptr, 16));
        data.push_back(byte);
    }

    return data;
}

auto forge::core::metadataStartsWithForgeId(const std::vector<std::byte>& metadata)
    -> bool
{
    if(metadata.size() < 3) {
        return false;
    }
    return std::equal(std::cbegin(FORGE_IDENTIFIER_MASK),
                      std::cend(FORGE_IDENTIFIER_MASK),
                      std::cbegin(metadata));
}

auto forge::core::toHexString(const std::vector<std::byte>& bytes)
    -> std::string
{
    std::stringstream ss;
    ss << std::hex;
    for(auto&& b : bytes) {
        ss << std::setw(2) << std::setfill('0') << static_cast<int>(b);
    }
    return ss.str();
}


auto forge::core::stringToASCIIByteVec(const std::string& str)
    -> std::vector<std::byte>
{
    std::vector<std::byte> byte_vec;
    std::transform(std::begin(str),
                   std::end(str),
                   std::back_inserter(byte_vec),
                   [](auto character) {
                       return static_cast<std::byte>(character);
                   });

    return byte_vec;
}
