#include <CLI/CLI.hpp>
#include <chrono>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/LookupOnlySubcommands.hpp>
#include <cli/ReadOnlySubcommands.hpp>
#include <cli/ReadWriteSubcommands.hpp>
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


using buddy::cli::addLookupOnlySubcommands;
using buddy::cli::addReadOnlySubcommands;
using buddy::cli::addReadWriteSubcommands;
using buddy::cli::RESPONSE;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;
using buddy::env::parseConfigFile;
using buddy::env::ProgramOptions;
using buddy::rpc::ReadWriteWalletStubClient;
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
    ReadWriteWalletStubClient client{httpclient, JSONRPC_CLIENT_V2};

    CLI::App app{"buddy-cli is a tool to easily communicate with a buddyd server via CLI"};

    app.set_help_all_flag("--help-all",
                          "Show all help");

    app.require_subcommand();

    addLookupOnlySubcommands(app, client);
    addReadOnlySubcommands(app, client);
    addReadWriteSubcommands(app, client);


    try {
        CLI11_PARSE(app, argc, argv);
        if(!RESPONSE.isNull()) {
            fmt::print("{}", RESPONSE.toStyledString());
        }
    } catch(JsonRpcException& e) {
        fmt::print("{}\n", e.what());
    }
}
