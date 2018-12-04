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
                                        std::size_t index,
                                        std::vector<std::byte>&& metadata,
                                        std::size_t burn_value,
                                        std::vector<
                                            std::pair<std::string,
                                                      std::size_t>>&& outputs) const
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

            // if(json.isMember("error")
            //    && !json["error"].isNull()) {
            //     return DaemonError{std::move(json["error"].asString())};
            // }

            // if(!json.isMember("result")
            //    && !json["result"].isString()) {
            //     return DaemonError{"unkown error while parsing result from tx creation"};
            // }

            // auto result = std::move(json["result"].asString());

            // auto byte_vec_opt = stringToByteVec(result));

            // if(!byte_vec_opt) {
            //     return DaemonError{"wasn't able to create byte vec from the result of a tx creation"};
            // }

            // return byte_vec_opt.getValue();
        });
}

auto ReadWriteOdinDaemon::generateRpcParams(std::string&& input_txid,
                                            std::size_t index,
                                            std::vector<std::byte>&& metadata,
                                            std::size_t burn_value,
                                            std::vector<
                                                std::pair<std::string,
                                                          std::size_t>>&& outputs) const
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
}

auto ReadWriteOdinDaemon::sendRawTx(std::vector<std::byte>&& tx) const
    -> Result<void, DaemonError>
{
    static const auto command = "sendrawtransaction"s;

    Json::Value params;
    params.append(toHexString(tx));
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
