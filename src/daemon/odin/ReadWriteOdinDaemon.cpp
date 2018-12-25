#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <daemon/odin/ReadWriteOdinDaemon.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::ReadWriteOdinDaemon;
using buddy::daemon::ReadOnlyOdinDaemon;
using utilxx::Opt;
using utilxx::Result;
using buddy::core::stringToByteVec;
using buddy::core::toHexString;
using namespace std::string_literals;


auto ReadWriteOdinDaemon::generateRawTx(std::string&& input_txid,
                                        std::int64_t index,
                                        std::vector<std::byte>&& metadata,
                                        std::int64_t burn_value,
                                        std::vector<
                                            std::pair<std::string,
                                                      std::int64_t>>&& outputs) const
    -> Result<std::vector<std::byte>,
              DaemonError>
{
    static const auto command = "createrawtransaction"s;

    auto params = generateRpcParams(std::move(input_txid),
                                    index,
                                    std::move(metadata),
                                    burn_value,
                                    std::move(outputs));

    return sendcommand(command, std::move(params))
        .flatMap([](auto&& json)
                     -> Result<std::vector<std::byte>,
                               DaemonError> {
            auto result = std::move(json.toStyledString());

            auto number_of_delimiters =
                std::count(std::begin(result),
                           std::end(result),
                           '"');

            if(number_of_delimiters < 2) {
                return DaemonError{"The result of \"createrawtransaction\" was way to short"};
            }

            auto first = result.find('"') + 1;
            auto last = result.find_last_of('"');
            result = result.substr(first, last - first);

            if(auto byte_vec_opt = stringToByteVec(result);
               byte_vec_opt) {
                return byte_vec_opt.getValue();
            } else {
                return DaemonError{std::move(result)};
            }
        });
}

auto ReadWriteOdinDaemon::generateRpcParams(std::string&& input_txid,
                                            std::int64_t index,
                                            std::vector<std::byte>&& metadata,
                                            std::int64_t burn_value,
                                            std::vector<
                                                std::pair<std::string,
                                                          std::int64_t>>&& outputs) const
    -> Json::Value
{
    auto metadata_str = toHexString(metadata);

    Json::Value input;
    input["txid"] = std::move(input_txid);
    input["vout"] = index;

    Json::Value inputs;
    inputs.append(std::move(input));

    Json::Value op_return;
    op_return["data"] = std::move(metadata_str);
    op_return["value"] = burn_value;

    Json::Value json_outputs;

    for(auto&& [address, value] : outputs) {
        Json::Value output;
        output["address"] = std::move(address);
        output["value"] = value;

        json_outputs.append(std::move(output));
    }

    json_outputs.append(std::move(op_return));

    Json::Value param;
    param.append(std::move(inputs));
    param.append(std::move(json_outputs));

    return param;
}


auto ReadWriteOdinDaemon::signRawTx(std::vector<std::byte>&& tx) const
    -> Result<std::vector<std::byte>,
              DaemonError>
{
    static const auto command = "signrawtransaction"s;

    Json::Value params;
    params.append(toHexString(tx));

    return sendcommand(command, params)
        .flatMap([](auto&& json)
                     -> Result<std::vector<std::byte>,
                               DaemonError> {
            //check if the request was complete
            if(!json.isMember("complete")
               || !json["complete"].isBool()
               || !json["complete"].asBool()) {

                if(!json.isMember("errors")
                   || !json["errors"].isArray()
                   || !json["errors"].isValidIndex(0)
                   || !json["errors"][0].isMember("error")
                   || !json["errors"][0]["error"].isString()) {
                    return DaemonError{std::move(json["errors"][0]["error"].asString())};
                } else {
                    return DaemonError{"unknown error during transaction signing"};
                }
            }

            if(!json.isMember("hex")
               || !json["hex"].isString()) {
                return DaemonError{"no hex value was returned from transaction signing"};
            }

            auto hex_str = std::move(json["hex"].asString());

            auto hex_vec = stringToByteVec(hex_str);

            if(!hex_vec) {
                return DaemonError{"wasn't able to create byte vec from the result of a transaction signing"};
            }

            return hex_vec.getValue();
        });
}

auto ReadWriteOdinDaemon::sendRawTx(std::vector<std::byte>&& tx) const
    -> Result<void, DaemonError>
{
    static const auto command = "sendrawtransaction"s;

    Json::Value params;
    params.append(toHexString(tx));
    //append to allow high fees
    params.append(true);

    return sendcommand(command, params)
        .flatMap([](auto&& json)
                     -> Result<void, DaemonError> {
            if(json.empty()) {
                return {};
            }

            return DaemonError{json.toStyledString()};
        });
}


auto ReadWriteOdinDaemon::generateNewAddress() const
    -> utilxx::Result<std::string, DaemonError>
{
    static const auto command = "getnewaddress"s;

    return sendcommand(command, {})
        .flatMap([](auto&& json)
                     -> Result<std::string, DaemonError> {
            //check if result is not there
            if(json.isNull() || !json.isString()) {
                return DaemonError{"unknown error while getting new address"};
            }

            return json.asString();
        });
}
