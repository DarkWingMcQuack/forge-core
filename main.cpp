#include <core/Operation.hpp>
#include <cxxopts.hpp>
#include <daemon/OdinDaemon.hpp>
#include <fmt/core.h>
#include <util/Opt.hpp>
#include <util/Result.hpp>

using buddy::daemon::OdinDaemon;
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
    auto daemon = daemon_from_args(argc, argv);

    auto block_res = daemon->getNewestBlock();

    if(block_res) {
        fmt::print("block hash: {}\n",
                   block_res.getValue().getHash());
    } else {
        fmt::print("{}\n",
                   block_res.getError().what());
    }
}
