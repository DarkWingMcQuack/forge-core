#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/OdinDaemon.hpp>
#include <fmt/core.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <lookup/LookupManager.hpp>
#include <thread>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::daemon::OdinDaemon;
using buddy::lookup::LookupManager;
using buddy::daemon::DaemonBase;
using buddy::daemon::make_daemon;
using buddy::daemon::Coin;

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

    auto args = parse_args(argc, argv);

    //setup logger
    auto logfile = std::get<0>(args);
    auto worker = g3::LogWorker::createLogWorker();
    worker->addDefaultLogger(argv[0],
                             logfile);
    // logger is initialized
    g3::initializeLogging(worker.get());



    auto daemon = make_daemon(std::get<1>(args),
                              std::get<2>(args),
                              std::get<3>(args),
                              std::get<4>(args),
                              Coin::Odin);
    LookupManager manager{std::move(daemon)};

    while(true) {

        auto res = manager.updateLookup();

        res.onError([](auto&& error) {
            fmt::print("{}\n",
                       std::visit(
                           [](auto&& er) {
                               return er.what();
                           },
                           error));
        });

        std::this_thread::sleep_for(2s);
    }
}
