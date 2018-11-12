#include <core/Block.hpp>
#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <daemon/odin/ReadWriteOdinDaemon.hpp>
#include <fmt/core.h>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::ReadWriteOdinDaemon;
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
}


auto ReadWriteOdinDaemon::signRawTx(std::vector<std::byte>&& tx) const
    -> Result<std::vector<std::byte>,
              DaemonError>
{
}

auto ReadWriteOdinDaemon::sendRawTx(std::vector<std::byte>&& tx) const
    -> Result<void, DaemonError>
{
    static const auto command = "validateaddress"s;

    Json::Value params;
    params.append(toHexString(tx));

    return sendcommand(command, params)
        .flatMap([](auto&& json)
                     -> Result<void, DaemonError> {
            if(json.empty()) {
                return {};
            }

            return DaemonError{json.toStyledString()};
        });
}

auto ReadWriteOdinDaemon::getScriptPubKeyOf(std::string&& address) const
    -> Result<std::vector<std::byte>,
              DaemonError>
{
    static const auto command = "validateaddress"s;

    Json::Value params;
    params.append(std::move(address));

    return sendcommand(command, params)
        .map([](auto&& json) {
            if(json.isMember("isvalid")
               && json.isMember("scriptPubKey")) {
                auto scriptPubKey = std::move(json["scriptPubKey"].asString());
                return Opt{std::move(scriptPubKey)};
            }

            return Opt<std::string>{std::nullopt};
        })
        .map([](auto&& opt) {
            return opt.flatMap([](auto&& value) {
                return stringToByteVec(std::move(value));
            });
        })
        .mapError([&params](auto&& error) {
            auto error_str =
                fmt::format("unable to build transaction from result when calling {}, with parameters {}\n",
                            command,
                            params.toStyledString());

            return DaemonError{std::move(error_str)};
        })
        .flatMap([](auto&& opt)
                     -> Result<std::vector<std::byte>,
                               DaemonError> {
            if(opt) {
                return opt.getValue();
            }

            return DaemonError{"unable to get scriptPubKey"};
        });
}
