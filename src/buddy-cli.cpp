#include <CLI/CLI.hpp>
#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <env/LoggingSetup.hpp>
#include <env/ProgramOptions.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <g3log/g3log.hpp>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <rpc/readwritewalletstubclient.h>
#include <thread>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>
#include <wallet/ReadWriteWallet.hpp>

using buddy::core::stringToByteVec;
using buddy::core::toHexString;
using buddy::core::Coin;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;
using buddy::env::parseConfigFile;
using buddy::env::ProgramOptions;
using buddy::rpc::ReadWriteWalletStubClient;
using jsonrpc::HttpClient;
using jsonrpc::JSONRPC_CLIENT_V2;
using jsonrpc::JsonRpcException;


std::string key = "";
bool is_string = false;
std::string owner = "";
std::string entry_value_str =
    "{\"type\" : \"none\","
    "\"value\":\"\"}";
Json::Value entry_value = Json::nullValue;
int burn_value = 0;


Json::Value response;

static auto parseIntoJson(const std::string& str)
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

static auto checkAndTransformValueString(const std::string& str)
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

static auto addLookupOnlySubcommands(CLI::App& app, ReadWriteWalletStubClient& client)
{
    app.add_subcommand("shutdown",
                       "stops the buddyd server")
        ->callback([&] {
            client.shutdown();
        });

    app.add_subcommand("updatelookup",
                       "updates the lookup to the newest block")
        ->callback([&] {
            response = client.updatelookup();
        });

    app.add_subcommand("rebuildlookup",
                       "rebuilds the lookup from the beginning")
        ->callback([&] {
            client.rebuildlookup();
        });

    app.add_subcommand("checkvalidity",
                       "checks if the lookup is valid in terms of blockhashes")
        ->callback([&] {
            response = client.checkvalidity();
        });

    auto lookupvalue_opt =
        app.add_subcommand("lookupvalue",
                           "looks up the value of a given byte vector/string")
            ->callback([&] {
                response = client.lookupvalue(is_string, key);
            });

    auto lookupowner_opt =
        app.add_subcommand("lookupowner",
                           "looks up the owner of an entry identified by a given byte vector/string key")
            ->callback([&] {
                response = client.lookupowner(is_string, key);
            });

    auto lookupactivationblock_opt =
        app.add_subcommand("lookupactivationblock",
                           "looks up the in which block the entry with the given key was last activated")
            ->callback([&] {
                response = client.lookupactivationblock(is_string, key);
            });

    auto lookupallentrysof_opt =
        app.add_subcommand("lookupallentrysof",
                           "returns a vector of all entrys the given owner currently owns")
            ->callback([&] {
                response = client.lookupallentrysof(owner);
            });

    lookupvalue_opt
        ->add_option("--key",
                     key,
                     "the key of which the value will be looked up")
        ->required();

    lookupvalue_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");

    lookupowner_opt
        ->add_option("--key",
                     key,
                     "the key of which the owner will be looked up")
        ->required();

    lookupowner_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");

    lookupactivationblock_opt
        ->add_option("--key",
                     key,
                     "the key of which the value will be looked up")
        ->required();

    lookupactivationblock_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");

    lookupallentrysof_opt
        ->add_option("--owner",
                     owner,
                     "owner address of which all the entrys will be looked up")
        ->required();
}

static auto addReadOnlySubcommands(CLI::App& app, ReadWriteWalletStubClient& client)
{
    app.add_subcommand("addwatchonlyaddress",
                       "adds a new address to watch")
        ->callback([&] {
            client.addwatchonlyaddress(owner);
        })
        ->add_option("--address", owner)
        ->required();

    app.add_subcommand("deletewatchonlyaddress",
                       "deletes an address from being watched")
        ->callback([&] {
            client.deletewatchonlyaddress(owner);
        })
        ->add_option("--address", owner)
        ->required();

    app.add_subcommand("addnewownedaddress",
                       "adds a new address which the wallet ownes")
        ->callback([&] {
            client.addwatchonlyaddress(owner);
        })
        ->add_option("--address", owner)
        ->required();

    app.add_subcommand("getownedentrys",
                       "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            response = client.getownedentrys();
        });

    app.add_subcommand("getwatchedonlyentrys",
                       "returns a list of all entrys the wallet observes but does not own")
        ->callback([&] {
            response = client.getwatchonlyentrys();
        });

    app.add_subcommand("getallwatchedentrys",
                       "returns a list of all entrys the wallet observes")
        ->callback([&] {
            response = client.getallwatchedentrys();
        });

    app.add_subcommand("getwatchedaddresses",
                       "returns a list of all addresses the wallet observes but does not own")
        ->callback([&] {
            response = client.getwatchedaddresses();
        });

    app.add_subcommand("getownedaddresses",
                       "returns a list of all addresses the wallet owns")
        ->callback([&] {
            response = client.getownedaddresses();
        });
}

static auto addReadWriteSubcommands(CLI::App& app, ReadWriteWalletStubClient& client)
{
    auto createnewentry_opt =
        app.add_subcommand("createnewentry",
                           "creates a new entry")
            ->callback([&] {
                entry_value = checkAndTransformValueString(entry_value_str);
                response = client.createnewentry(owner, burn_value, is_string, key, entry_value);
            });


    app.require_subcommand();

    createnewentry_opt
        ->add_option("--key",
                     key,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--value",
                     entry_value_str,
                     "value the entry holds, expected to be a json object\n"
                     "with valid \"type\" and \"value\" fields.\n"
                     "If not given, the entry will hold \"none\" as value");

    createnewentry_opt
        ->add_option("--burn-value",
                     burn_value,
                     "number of coins which will be burned")
        ->required();

    createnewentry_opt
        ->add_option("--address",
                     owner,
                     "optional address which will be used as owner of the new entry,\n"
                     "be sure the used wallet ownes the private key to this address.\n"
                     "If not given, then a new address will be generated");
}




auto main(int argc, char* argv[]) -> int
{
    using namespace std::string_literals;

    static const auto default_buddy_dir = getenv("HOME") + "/.buddy"s;

    auto params = parseConfigFile(default_buddy_dir);
    auto port = params.getRpcPort();
    HttpClient httpclient("http://localhost:" + std::to_string(port));
    ReadWriteWalletStubClient client{httpclient, JSONRPC_CLIENT_V2};

    CLI::App app{"buddy-cli is a tool to easily communicate with a buddyd server via CLI"};

    app.set_help_all_flag("--help-all",
                          "Show all help");

    addLookupOnlySubcommands(app, client);
    addReadOnlySubcommands(app, client);
    addReadWriteSubcommands(app, client);


    try {
        CLI11_PARSE(app, argc, argv);
        if(!response.isNull()) {
            fmt::print("{}", response.toStyledString());
        }
    } catch(JsonRpcException& e) {
        fmt::print("{}\n", e.what());
    }
}
