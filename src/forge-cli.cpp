#include "rpc/jsonrpcstubclient.h"
#include <CLI/CLI.hpp>
#include <chrono>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/LookupOnlySubcommands.hpp>
#include <cli/ReadOnlySubcommands.hpp>
#include <cli/ReadWriteSubcommands.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
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
#include <rpc/jsonrpcstubclient.h>
#include <thread>
#include <utils/Opt.hpp>
#include <utils/Result.hpp>
#include <wallet/ReadWriteWallet.hpp>


using forge::cli::addLookupOnlySubcommands;
using forge::cli::addReadOnlySubcommands;
using forge::cli::addReadWriteSubcommands;
using forge::cli::RESPONSE;
using forge::env::parseConfigFile;
using forge::rpc::JsonRpcStubClient;
using jsonrpc::HttpClient;
using jsonrpc::JSONRPC_CLIENT_V2;
using jsonrpc::JsonRpcException;


auto main(int argc, char* argv[]) -> int
{
    using namespace std::string_literals;

    static auto default_forge_dir = getenv("HOME") + "/.forge"s;


    CLI::App app{"forge-cli is a tool to easily communicate with a forged server via CLI"};

    app.set_help_all_flag("--help-all",
                          "Show all help");

    app.add_option("--workdir", default_forge_dir);

    auto params = parseConfigFile(default_forge_dir);
    auto port = params.getRpcPort();
    HttpClient httpclient("http://localhost:" + std::to_string(port));
    JsonRpcStubClient client{httpclient, JSONRPC_CLIENT_V2};

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
