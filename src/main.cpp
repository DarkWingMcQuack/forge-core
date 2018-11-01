#include <chrono>
#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/OdinDaemon.hpp>
#include <fmt/core.h>
#include <lookup/LookupManager.hpp>
#include <thread>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::daemon::OdinDaemon;
using buddy::lookup::LookupManager;
using buddy::daemon::DaemonBase;
using buddy::daemon::make_daemon;
using buddy::daemon::Coin;

auto daemon_from_args(int argc, char* argv[])
    -> std::unique_ptr<DaemonBase>
{
    cxxopts::Options options("cppBUDDY", "Implementation of the BUDDY protocol");

    // clang-format off
    options.add_options()
        ("help", "Print help and exit.")
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

        auto user = result["user"].as<std::string>();
        auto password = result["password"].as<std::string>();
        auto host = result["host"].as<std::string>();
        auto port = result["port"].as<std::size_t>();

        auto daemon = make_daemon(std::move(host),
                                  std::move(user),
                                  std::move(password),
                                  port,
                                  Coin::Odin);
        return daemon;

    } catch(const std::exception& e) {
        fmt::print("{}\n", e.what());
        std::exit(-1);
    }
}

auto main(int argc, char* argv[]) -> int
{
    using namespace std::chrono_literals;

    auto daemon = daemon_from_args(argc, argv);

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
