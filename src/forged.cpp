#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <client/WriteOnlyClientBase.hpp>
#include <client/odin/ReadOnlyOdinClient.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <env/LoggingSetup.hpp>
#include <env/ProgramOptions.hpp>
#include <fcntl.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <getopt.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/JsonRpcServer.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <thread>
#include <unistd.h>
#include <utils/Opt.hpp>
#include <utils/Result.hpp>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>

using forge::lookup::LookupManager;
using forge::client::make_readonly_client;
using forge::client::make_writing_client;
using forge::wallet::ReadWriteWallet;
using forge::wallet::ReadOnlyWallet;
using forge::env::initConsoleLogger;
using forge::env::initFileLogger;
using forge::env::parseOptions;
using forge::env::ProgramOptions;
using forge::rpc::JsonRpcServer;
using jsonrpc::HttpServer;
using jsonrpc::JSONRPC_SERVER_V1V2;

static void clientize()
{
    pid_t pid = 0;
    int fd;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if(pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if(setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Ignore signal sent from child to parent process */
    signal(SIGCHLD, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if(pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    for(fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
        close(fd);
    }

    /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");
}

auto assertOnMainnet(const forge::client::ReadOnlyClientBase& client)
{
    auto is_main_res = client.isMainnet();

    if(is_main_res.hasError()) {
        fmt::print("{}\n", is_main_res.getError().what());
        std::exit(-1);
    }

    auto is_main = is_main_res.getValue();

    if(is_main) {
        fmt::print("forge-core discovered that you are trying to use it on mainnet\n");
        fmt::print(
            "forge-core is currently highly unstable software "
            "please only connect forge core with testnet wallets\n");
        std::exit(0);
    }
}

auto runLookupOnlyServer(const ProgramOptions& params)
{
    auto client = make_readonly_client(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());
    assertOnMainnet(*client);

    auto port = params.getRpcPort();
    auto threads = params.getNumberOfThreads();


    HttpServer httpserver{static_cast<int>(port),
                          "",
                          "",
                          static_cast<int>(threads)};

    LookupManager lookup{std::move(client)};

    JsonRpcServer rpcserver{httpserver,
                            JSONRPC_SERVER_V1V2,
                            std::move(lookup)};
    rpcserver.StartListening();

    forge::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto runReadOnlyWalletServer(const ProgramOptions& params)
{
    auto client = make_readonly_client(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());

    assertOnMainnet(*client);

    auto lookup = std::make_unique<LookupManager>(std::move(client));
    ReadOnlyWallet wallet{std::move(lookup)};

    auto port = params.getRpcPort();
    auto threads = params.getNumberOfThreads();


    HttpServer httpserver{static_cast<int>(port),
                          "",
                          "",
                          static_cast<int>(threads)};

    JsonRpcServer rpcserver{httpserver,
                            JSONRPC_SERVER_V1V2,
                            std::move(wallet)};
    rpcserver.StartListening();

    forge::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto runReadWriteWalletServer(const ProgramOptions& params)
{
    auto reader = make_readonly_client(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());

    assertOnMainnet(*reader);

    auto writer = make_writing_client(params.getCoinHost(),
                                      params.getCoinUser(),
                                      params.getCoinPassword(),
                                      params.getCoinPort(),
                                      params.getCoin());

    auto lookup = std::make_unique<LookupManager>(std::move(reader));
    ReadWriteWallet wallet{std::move(lookup),
                           std::move(writer)};

    auto port = params.getRpcPort();
    auto threads = params.getNumberOfThreads();


    HttpServer httpserver{static_cast<int>(port),
                          "",
                          "",
                          static_cast<int>(threads)};

    JsonRpcServer rpcserver{httpserver,
                            JSONRPC_SERVER_V1V2,
                            std::move(wallet)};
    rpcserver.StartListening();

    forge::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto main(int argc, char* argv[]) -> int
{
    using namespace std::chrono_literals;

    auto params = parseOptions(argc, argv);

    //we need to first clientize because of
    //https://github.com/KjellKod/g3log/issues/151
    if(params.shouldClientize()) {
        fmt::print("Starting forge client, listening on port {}\n",
                   params.getRpcPort());
        clientize();
    }

    if(!params.shouldLogToConsole()) {
        initFileLogger(argv[0],
                       params.getLogFolder());
    } else {
        initConsoleLogger();
    }


    switch(params.getMode()) {

    case forge::env::Mode::LookupOnly:
        runLookupOnlyServer(params);
        break;

    case forge::env::Mode::ReadOnly:
        runReadOnlyWalletServer(params);
        break;

    case forge::env::Mode::ReadWrite:
        runReadWriteWalletServer(params);
        break;
    }
}
