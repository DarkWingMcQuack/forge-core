#include <array>
#include <boost/algorithm/hex.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <daemon/DaemonBase.hpp>
#include <json/value.h>
#include <util/Opt.hpp>
#include <vector>

using buddy::core::TxIn;
using buddy::core::TxOut;
using buddy::core::Transaction;
using buddy::daemon::DaemonBase;
using buddy::daemon::DaemonError;
using buddy::util::Result;
using buddy::util::Opt;
using buddy::util::traverse;
using buddy::core::BUDDY_IDENTIFIER_MASK;
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
    auto forward_iter = std::find_if(std::cbegin(outputs_),
                                     std::cend(outputs_),
                                     [](auto&& out) {
                                         return out.isOpReturnOutput();
                                     });

    auto backward_iter = std::find_if(std::crbegin(outputs_),
                                      std::crend(outputs_),
                                      [](auto&& out) {
                                          return out.isOpReturnOutput();
                                      });

    return forward_iter == backward_iter.base() - 1
        && forward_iter != std::cend(outputs_);
}

auto Transaction::getFirstOpReturnOutput() const
    -> util::Opt<std::reference_wrapper<const TxOut>>
{
    using Ret = util::Opt<std::reference_wrapper<const TxOut>>;

    auto iter = std::find_if(std::cbegin(outputs_),
                             std::cend(outputs_),
                             [](auto&& out) {
                                 return out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::cref(*iter)};
}

auto Transaction::getFirstOpReturnOutput()
    -> util::Opt<std::reference_wrapper<TxOut>>
{
    using Ret = util::Opt<std::reference_wrapper<TxOut>>;

    auto iter = std::find_if(std::begin(outputs_),
                             std::end(outputs_),
                             [](auto&& out) {
                                 return out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::ref(*iter)};
}

auto Transaction::getFirstNonOpReturnOutput() const
    -> util::Opt<std::reference_wrapper<const TxOut>>
{
    using Ret = util::Opt<std::reference_wrapper<const TxOut>>;

    auto iter = std::find_if(std::cbegin(outputs_),
                             std::cend(outputs_),
                             [](auto&& out) {
                                 return !out.isOpReturnOutput();
                             });

    return iter == std::cend(outputs_)
        ? Ret{}
        : Ret{std::cref(*iter)};
}

auto Transaction::getFirstNonOpReturnOutput()
    -> util::Opt<std::reference_wrapper<TxOut>>
{
    using Ret = util::Opt<std::reference_wrapper<TxOut>>;

    auto iter = std::find_if(std::begin(outputs_),
                             std::end(outputs_),
                             [](auto&& out) {
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
    -> std::size_t
{
    return outputs_.size();
}

auto buddy::core::buildTxIn(Json::Value&& json)
    -> util::Opt<TxIn>
{
    try {
        auto txid = std::move(json["txid"].asString());
        auto vout_index = json["vout"].asUInt();

        return TxIn{std::move(txid),
                    vout_index};
    } catch(...) {
        return std::nullopt;
    }
}

auto buddy::core::buildTxOut(Json::Value&& json)
    -> util::Opt<TxOut>
{
    try {
        auto json_vec = std::move(json["scriptPubKey"]["addresses"]);
        auto hex = std::move(json["scriptPubKey"]["hex"].asString());
        auto value = json["value"].asUInt();

        std::vector<std::string> addresses;
        std::transform(std::cbegin(json_vec),
                       std::cend(json_vec),
                       std::back_inserter(addresses),
                       [](auto&& value) {
                           return std::move(value.asString());
                       });

        return TxOut{value,
                     std::move(hex),
                     std::move(addresses)};
    } catch(...) {
        return std::nullopt;
    }
}

auto buddy::core::buildTransaction(Json::Value&& json)
    -> util::Opt<Transaction>
{
    try {
        auto txid = std::move(json["txid"].asString());
        auto vin = std::move(json["vin"]);
        auto vout = std::move(json["vout"]);

        auto inputs =
            traverse(std::vector<Json::Value>(vin.begin(),
                                              vin.end()),
                     [](auto&& input) {
                         return buildTxIn(std::move(input));
                     });
        auto outputs =
            traverse(std::vector<Json::Value>(vout.begin(),
                                              vout.end()),
                     [](auto&& output) {
                         return buildTxOut(std::move(output));
                     });

        return combine(std::move(inputs),
                       std::move(outputs))
            .map([&txid](auto&& pair) {
                return Transaction{std::move(pair.first),
                                   std::move(pair.second),
                                   std::move(txid)};
            });
    } catch(...) {
        return std::nullopt;
    }
}

auto buddy::core::extractMetadata(std::string&& hex)
    -> util::Opt<std::vector<std::byte>>
{
    if(hex.size() < 13) {
        return std::nullopt;
    }

    //remove the op return Opcode
    //and the next byte
    if(hex[0] != '6' || hex[1] != 'a') {
        return std::nullopt;
    }
    hex.erase(0, 4);

    return stringToByteVec(std::move(hex));
}

auto buddy::core::stringToByteVec(std::string&& str)
    -> util::Opt<std::vector<std::byte>>
{
    std::vector<unsigned char> raw_data;
    try {
        boost::algorithm::hex(std::cbegin(str),
                              std::cend(str),
                              std::back_inserter(raw_data));
    } catch(...) {
        return std::nullopt;
    }

    std::vector<std::byte> data;
    std::transform(std::cbegin(raw_data),
                   std::cend(raw_data),
                   std::back_inserter(data),
                   [](auto&& ch) {
                       return static_cast<std::byte>(ch);
                   });

    return data;
}

auto buddy::core::metadataStartsWithBuddyId(const std::vector<std::byte>& metadata)
    -> bool
{
    return std::equal(std::cbegin(BUDDY_IDENTIFIER_MASK),
                      std::cend(BUDDY_IDENTIFIER_MASK),
                      std::cbegin(metadata));
}
