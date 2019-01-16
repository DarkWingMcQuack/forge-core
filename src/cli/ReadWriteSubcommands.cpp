#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/ReadWriteSubcommands.hpp>
#include <core/Entry.hpp>
#include <core/Transaction.hpp>
#include <fmt/core.h>
#include <rpc/readwritewalletstubclient.h>

using buddy::core::stringToByteVec;
using buddy::core::stringToASCIIByteVec;


namespace {

auto parseIntoJson(const std::string& str)
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        str.c_str(),
        str.c_str() + str.size(),
        &json,
        &errors);
    delete reader;

    return json;
}

auto checkAndTransformValueString(const std::string& str)
    -> Json::Value
{
    auto json = parseIntoJson(str);

    if(!json.isMember("type")
       || !json["type"].isString()) {
        fmt::print("given json value {}, is invalid, the json object needs a string field with name \"type\"\n",
                   json.toStyledString());
        std::exit(0);
    }

    const auto& type = json["type"].asString();

    if(type != "ipv4"
       && type != "ipv6"
       && type != "bytearray"
       && type != "none") {

        fmt::print(
            "in value json object only the type \"ipv4\", \"ipv6\", "
            "\"bytearray\" or \"none\" are allowed, currently given {}\n",
            type);
        std::exit(0);
    }


    if(type == "ipv4") {
        const auto& value = json["value"].asString();
        if(value.size() != 8) {
            fmt::print("given ipv4 value has invalid length of {}, but should have 8\n",
                       value.size());
            std::exit(0);
        }

        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector\n",
                       value);
            std::exit(0);
        }

    } else if(type == "ipv6") {
        const auto& value = json["value"].asString();
        if(value.size() != 32) {
            fmt::print("given ipv6 value has invalid length of {}, but should have 8\n",
                       value.size());
            std::exit(0);
        }

        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector\n",
                       value);
            std::exit(0);
        }

    } else if(type == "bytevec") {
        const auto& value = json["value"].asString();
        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector",
                       value);
            std::exit(0);
        }
    } else if(type == "none"
              && json.isMember("value")) {

        if(!json["value"].isString()
           || !json["value"].asString().empty()) {
            fmt::print("since type of value is \"none\", given value {} will be ignored\n",
                       json["value"].toStyledString());
        }
    }

    return json;
}

} // namespace

auto buddy::cli::addCreateEntry(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto createnewentry_opt =
        app.add_subcommand("createnewentry",
                           "creates a new entry")
            ->callback([&] {
                ENTRY_VALUE = checkAndTransformValueString(ENTRY_VALUE_STR);
                RESPONSE = client.createnewentry(OWNER, BURN_VALUE, IS_STRING, KEY, ENTRY_VALUE);
            });

    createnewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--value",
                     ENTRY_VALUE_STR,
                     "value the entry holds, expected to be a json object "
                     "with valid \"type\" and \"value\" fields. "
                     "If not given, the entry will hold \"none\" as value");

    createnewentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    createnewentry_opt
        ->add_option("--address",
                     OWNER,
                     "optional address which will be used as owner of the new entry, "
                     "be sure the used wallet ownes the private key to this address. "
                     "If not given, then a new address will be generated");
}

auto buddy::cli::addPayToEntry(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto createnewentry_opt =
        app.add_subcommand("paytoentry",
                           "sends a given number of coins to the owner of a given entry")
            ->callback([&] {
                RESPONSE = client.paytoentryowner(BURN_VALUE, IS_STRING, KEY);
            });

    createnewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--amount",
                     BURN_VALUE,
                     "number of coins which will be send")
        ->required();
}


auto buddy::cli::addReadWriteSubcommands(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    addCreateEntry(app, client);
    addPayToEntry(app, client);
}
