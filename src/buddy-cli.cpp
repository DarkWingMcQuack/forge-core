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
#include <jsonrpccpp/client/connectors/httpclient.h>
#include <rpc/lookuponlystubclient.h>
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
using buddy::rpc::LookupOnlyStubClient;
using jsonrpc::HttpClient;
using jsonrpc::JSONRPC_CLIENT_V2;
using jsonrpc::JsonRpcException;


auto main(int argc, char* argv[]) -> int
{
    using namespace std::string_literals;

    static const auto default_buddy_dir = getenv("HOME") + "/.buddy"s;

    auto params = parseConfigFile(default_buddy_dir);
    auto port = params.getRpcPort();
    HttpClient httpclient("http://localhost:" + std::to_string(port));
    LookupOnlyStubClient client{httpclient, JSONRPC_CLIENT_V2};

    Json::Value response;

    CLI::App app{"buddy-cli is a tool to easily communicate with a buddyd server via CLI"};

    std::string key;
    bool is_string;
    std::string owner;

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
                           "looks up the value of a given byte vector/string")
            ->callback([&] {
                response = client.lookupowner(is_string, key);
            });

    auto lookupallentrysof_opt =
        app.add_subcommand("lookupallentrysof",
                           "returns a vector of all entrys the given owner currently owns")
            ->callback([&] {
                response = client.lookupallentrysof(owner);
            });


    app.require_subcommand();

    lookupvalue_opt
        ->add_option("--key",
                     key,
                     "the key of which the value will be looked up");
    // ->required();

    lookupvalue_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");
    // ->required();

    lookupowner_opt
        ->add_option("--key",
                     key,
                     "the key of which the owner will be looked up");
    // ->required();

    lookupowner_opt
        ->add_flag("--isstring",
                   is_string,
                   "if set, the given key will be interpreted as string and not as byte vector");
    // ->required();

    lookupallentrysof_opt
        ->add_option("--owner",
                     owner,
                     "owner address of which all the entrys will be looked up");
    // ->required();



    try {
        app.parse(argc, argv);
        fmt::print("{}", response.toStyledString());
    } catch(JsonRpcException& e) {
        fmt::print("{}", e.what());
    }
}
