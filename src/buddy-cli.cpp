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
#include <rpc/readonlywalletstubclient.h>
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
using buddy::rpc::ReadOnlyWalletStubClient;
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
    ReadOnlyWalletStubClient client{httpclient, JSONRPC_CLIENT_V2};

    Json::Value response;

    CLI::App app{"buddy-cli is a tool to easily communicate with a buddyd server via CLI"};

    std::string key;
    bool is_string;
    std::string owner;

    app.set_help_all_flag("--help-all",
                          "Show all help");

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


    app.require_subcommand();

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



    try {
        CLI11_PARSE(app, argc, argv);
        if(!response.isNull()) {
            fmt::print("{}", response.toStyledString());
        }
    } catch(JsonRpcException& e) {
        fmt::print("{}\n", e.what());
    }
}
