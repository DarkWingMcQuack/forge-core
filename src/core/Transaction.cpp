#include <array>
#include <boost/algorithm/hex.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <json/value.h>
#include <util/Opt.hpp>
#include <vector>

using buddy::core::TxIn;
using buddy::core::TxOut;
using buddy::core::Transaction;
using buddy::core::BUDDY_IDENTIFIER_MASK;
using namespace std::string_literals;


TxIn::TxIn(std::string&& txid,
           std::size_t vout_index)
    : txid_(std::move(txid)),
      vout_index_(vout_index) {}


TxIn::TxIn(Json::Value&& json)
{
    txid_ = std::move(json["txid"].asString());
    vout_index_ = json["vout"].asUInt();
}


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

TxOut::TxOut(Json::Value&& json)
{
    auto json_vec = std::move(json["scriptPubKey"]["addresses"]);
    hex_ = std::move(json["scriptPubKey"]["hex"].asString());
    value_ = json["value"].asUInt();

    std::transform(std::cbegin(json_vec),
                   std::cend(json_vec),
                   std::back_inserter(addresses_),
                   [](auto&& value) {
                       return std::move(value.asString());
                   });
}


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


Transaction::Transaction(Json::Value&& json)
{
    txid_ = std::move(json["txid"].asString());
    auto vin = std::move(json["vin"]);
    auto vout = std::move(json["vout"]);

    //get inputs
    std::transform(std::begin(vin),
                   std::end(vin),
                   std::back_inserter(inputs_),
                   [](auto&& input) {
                       return TxIn{std::move(input)};
                   });

    //get outputs
    std::transform(std::begin(vout),
                   std::end(vout),
                   std::back_inserter(outputs_),
                   [](auto&& output) {
                       return TxOut{std::move(output)};
                   });
}

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

auto Transaction::getFirstOpReturnOutput() const
    -> util::Opt<std::reference_wrapper<const TxOut>>
{
    for(auto&& output : outputs_) {
        if(output.isOpReturnOutput()) {
            return std::cref(output);
        }
    }

    return std::nullopt;
}

auto Transaction::getFirstOpReturnOutput()
    -> util::Opt<std::reference_wrapper<TxOut>>
{
    for(auto&& output : outputs_) {
        if(output.isOpReturnOutput()) {
            return std::ref(output);
        }
    }

    return std::nullopt;
}

auto Transaction::getFirstNonOpReturnOutput() const
    -> util::Opt<std::reference_wrapper<const TxOut>>
{
    for(auto&& output : outputs_) {
        if(!output.isOpReturnOutput()) {
            return std::cref(output);
        }
    }

    return std::nullopt;
}

auto Transaction::getFirstNonOpReturnOutput()
    -> util::Opt<std::reference_wrapper<TxOut>>
{
    for(auto&& output : outputs_) {
        if(!output.isOpReturnOutput()) {
            return std::ref(output);
        }
    }

    return std::nullopt;
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
