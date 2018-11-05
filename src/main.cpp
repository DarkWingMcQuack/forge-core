#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/OdinDaemon.hpp>
#include <env/LoggingSetup.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <lookup/LookupManager.hpp>
#include <thread>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using buddy::daemon::OdinDaemon;
using buddy::lookup::LookupManager;
using buddy::daemon::DaemonBase;
using buddy::daemon::make_daemon;
using buddy::daemon::Coin;
using buddy::env::initConsoleLogger;
using buddy::env::initFileLogger;

auto parse_args(int argc, char* argv[])
    -> std::tuple<std::string,
                  std::string,
                  std::string,
                  std::string,
                  std::size_t>
{
    cxxopts::Options options("cppBUDDY", "Implementation of the BUDDY protocol");

    // clang-format off
    options.add_options()
        ("help", "Print help and exit.")
        ("l,logpath", "logpath",cxxopts::value<std::string>()->default_value(""))
        ("u,user", "daemon RPC user",cxxopts::value<std::string>())
        ("x,password", "daemon RPC password",cxxopts::value<std::string>())
        ("h,host", "hostname of the daemon",cxxopts::value<std::string>())
        ("p,port", "port of the RPC daemon",cxxopts::value<std::size_t>());
    // clang-format on

    try {
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            fmt::print("{}\n", options.help());
            exit(0);
        }

        auto logfile = result["logpath"].as<std::string>();
        auto user = result["user"].as<std::string>();
        auto password = result["password"].as<std::string>();
        auto host = result["host"].as<std::string>();
        auto port = result["port"].as<std::size_t>();

        return {std::move(logfile),
                std::move(user),
                std::move(password),
                std::move(host),
                port};

    } catch(const std::exception& e) {
        fmt::print("{}\n", e.what());
        std::exit(-1);
    }
}

auto main(int argc, char* argv[]) -> int
{
    using namespace std::chrono_literals;

    //parse comandline ards
    auto [log_folder,
          user,
          password,
          host,
          port] = parse_args(argc, argv);

    //if no path was given, use the console
    if(log_folder.empty()) {
        initConsoleLogger();
    } else {
        initFileLogger(argv[0],
                       log_folder);
    }


    //get a daemon
    auto daemon = make_daemon(host,
                              user,
                              password,
                              port,
                              Coin::Odin);

    LookupManager manager{std::move(daemon)};

    while(true) {

        //update to newest block
        auto res = manager.updateLookup();
        //report errors

        res.onError([](auto&& error) {
               auto error_msg = std::visit(
                   [](auto&& er) {
                       return er.what();
                   },
                   error);
               LOG(WARNING) << error_msg;
           })
            .onValue([&](auto&& added) {
                if(!added) {
                    return;
                }

                auto valid_res = manager.lookupIsValid();

                valid_res.onError([](auto&& error) {
                             LOG(WARNING) << error.what();
                         })
                    .onValue([](auto&& value) {
                        LOG_IF(DEBUG, value) << "lookup is valid";
                        LOG_IF(WARNING, !value) << "lookup is invalid";
                    });
            });

        //do this every 30 seconds
        std::this_thread::sleep_for(30s);
    }
}
