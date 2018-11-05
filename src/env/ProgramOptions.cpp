#include <cpptoml.h>
#include <cxxopts.hpp>
#include <env/ProgramOptions.hpp>
#include <fmt/core.h>
#include <fstream>
#include <g3log/g3log.hpp>
#include <string>
#include <utilxx/Opt.hpp>

using buddy::env::ProgramOptions;

ProgramOptions::ProgramOptions(utilxx::Opt<std::string>&& logfolder,
                               std::size_t coin_port,
                               std::string&& coin_host,
                               std::string&& coin_user,
                               std::string&& coin_password,
                               std::size_t rpc_port,
                               std::string&& rpc_user,
                               std::string&& rpc_password)
    : logfolder_(std::move(logfolder)),
      coin_port_(coin_port),
      coin_host_(std::move(coin_host)),
      coin_user_(std::move(coin_user)),
      coin_password_(std::move(coin_password)),
      rpc_port_(rpc_port),
      rpc_user_(std::move(rpc_user)),
      rpc_password_(std::move(rpc_password)) {}

auto ProgramOptions::getLogFolder() const
    -> const utilxx::Opt<std::string>
{
    return logfolder_;
}

auto ProgramOptions::getCoinPort() const
    -> std::size_t
{
    return coin_port_;
}

auto ProgramOptions::getCoinHost() const
    -> const std::string&
{
    return coin_host_;
}

auto ProgramOptions::getCoinUser() const
    -> const std::string&
{
    return coin_user_;
}

auto ProgramOptions::getCoinPassword() const
    -> const std::string&
{
    return coin_password_;
}

auto ProgramOptions::getRpcPort() const
    -> std::size_t
{
    return rpc_port_;
}

auto ProgramOptions::getRpcUser() const
    -> const std::string&
{
    return rpc_user_;
}

auto ProgramOptions::getRpcPassword() const
    -> const std::string&
{
    return rpc_password_;
}


auto buddy::env::parseOptions(int argc, char* argv[])
    -> ProgramOptions
{
    using namespace std::string_literals;

    cxxopts::Options options("cppBUDDY", "Implementation of the BUDDY protocol");

    static auto default_buddy_dir = getenv("HOME") + "/.buddy"s;

    // clang-format off
    options.add_options()
        ("help", "Print help and exit.")
        ("w,workdir", "path to the workingfolder of buddy",
         cxxopts::value<std::string>()->default_value(default_buddy_dir))
        ("l,log", "if set, logging will be displayed on the terminal",
         cxxopts::value<bool>()->default_value("false"));
    // clang-format on

    try {
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            fmt::print("{}\n", options.help());
            std::exit(0);
        }

        auto config_path = result["workdir"].as<std::string>();
        auto log_to_console = result["log"].as<bool>();

        auto config = cpptoml::parse_file(config_path + "/buddy.conf");
        auto log_path = config->get_qualified_as<std::string>("log-folder").value_or(default_buddy_dir + "/log/");
        auto coin_port = *config->get_qualified_as<std::size_t>("coin.port");
        auto coin_host = *config->get_qualified_as<std::string>("coin.host");
        auto coin_user = *config->get_qualified_as<std::string>("coin.user");
        auto coin_password = *config->get_qualified_as<std::string>("coin.password");
        auto rpc_port = config->get_qualified_as<std::size_t>("rpc.port").value_or(25000);
        auto rpc_user = config->get_qualified_as<std::string>("rpc.user").value_or("user");
        auto rpc_password = config->get_qualified_as<std::string>("rpc.password").value_or("password");

        if(log_to_console) {
            return ProgramOptions{std::nullopt,
                                  coin_port,
                                  std::move(coin_host),
                                  std::move(coin_user),
                                  std::move(coin_password),
                                  rpc_port,
                                  std::move(rpc_user),
                                  std::move(rpc_password)};
        }

        return ProgramOptions{std::move(log_path),
                              coin_port,
                              std::move(coin_host),
                              std::move(coin_user),
                              std::move(coin_password),
                              rpc_port,
                              std::move(rpc_user),
                              std::move(rpc_password)};


    } catch(const std::exception& e) {
        fmt::print("{}\n", e.what());
        fmt::print("Something went wrong with the parameters or the config file\n");
        std::exit(-1);
    }
}
