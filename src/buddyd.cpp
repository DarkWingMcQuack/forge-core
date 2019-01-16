#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/WriteOnlyDaemonBase.hpp>
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <env/LoggingSetup.hpp>
#include <env/ProgramOptions.hpp>
#include <errno.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <getopt.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/LookupOnlyServer.hpp>
#include <rpc/ReadOnlyWalletServer.hpp>
#include <rpc/ReadWriteWalletServer.hpp>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <thread>
#include <unistd.h>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>

using buddy::lookup::LookupManager;
using buddy::daemon::ReadOnlyDaemonBase;
using buddy::daemon::make_readonly_daemon;
using buddy::daemon::make_writing_daemon;
using buddy::wallet::ReadWriteWallet;
using buddy::wallet::ReadOnlyWallet;
using buddy::core::stringToByteVec;
using buddy::core::toHexString;
using buddy::core::Coin;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;
using buddy::env::parseOptions;
using buddy::env::ProgramOptions;
using buddy::rpc::LookupOnlyServer;
using buddy::rpc::ReadOnlyWalletServer;
using buddy::rpc::ReadWriteWalletServer;
using jsonrpc::HttpServer;
using jsonrpc::JSONRPC_SERVER_V1V2;

static void daemonize()
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

auto runLookupOnlyServer(const ProgramOptions& params)
{
    auto daemon = make_readonly_daemon(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());

    auto port = params.getRpcPort();
    auto threads = params.getNumberOfThreads();


    HttpServer httpserver{static_cast<int>(port),
                          "",
                          "",
                          static_cast<int>(threads)};

    LookupOnlyServer rpcserver{httpserver,
                               JSONRPC_SERVER_V1V2,
                               std::move(daemon)};
    rpcserver.StartListening();

    buddy::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto runReadOnlyWalletServer(const ProgramOptions& params)
{
    auto daemon = make_readonly_daemon(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());

    auto lookup = std::make_unique<LookupManager>(std::move(daemon));
    ReadOnlyWallet wallet{std::move(lookup)};

    auto port = params.getRpcPort();
    auto threads = params.getNumberOfThreads();


    HttpServer httpserver{static_cast<int>(port),
                          "",
                          "",
                          static_cast<int>(threads)};

    ReadOnlyWalletServer rpcserver{httpserver,
                                   JSONRPC_SERVER_V1V2,
                                   std::move(wallet)};
    rpcserver.StartListening();

    buddy::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto runReadWriteWalletServer(const ProgramOptions& params)
{
    auto reader = make_readonly_daemon(params.getCoinHost(),
                                       params.getCoinUser(),
                                       params.getCoinPassword(),
                                       params.getCoinPort(),
                                       params.getCoin());

    auto writer = make_writing_daemon(params.getCoinHost(),
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

    ReadWriteWalletServer rpcserver{httpserver,
                                    JSONRPC_SERVER_V1V2,
                                    std::move(wallet)};
    rpcserver.StartListening();

    buddy::rpc::waitForShutdown(rpcserver);

    rpcserver.StopListening();
}

auto main(int argc, char* argv[]) -> int
{
    using namespace std::chrono_literals;

    auto params = parseOptions(argc, argv);

    //we need to first daemonize because of
    //https://github.com/KjellKod/g3log/issues/151
    if(params.shouldDaemonize()) {
        fmt::print("Starting buddyd daemon, listening on port {}\n", params.getRpcPort());
        daemonize();
    }

    if(!params.shouldLogToConsole()) {
        initFileLogger(argv[0],
                       params.getLogFolder());
    } else {
        initConsoleLogger();
    }


    switch(params.getMode()) {

    case buddy::env::Mode::LookupOnly:
        runLookupOnlyServer(params);
        break;

    case buddy::env::Mode::ReadOnly:
        runReadOnlyWalletServer(params);
        break;

    case buddy::env::Mode::ReadWrite:
        runReadWriteWalletServer(params);
        break;
    }
}
