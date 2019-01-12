#include <cpptoml.h>
#include <cxxopts.hpp>
#include <env/ProgramOptions.hpp>
#include <fmt/core.h>
#include <fstream>
#include <g3log/g3log.hpp>
#include <string>
#include <utilxx/Opt.hpp>

using buddy::env::ProgramOptions;

ProgramOptions::ProgramOptions(std::string&& logfolder,
                               Mode mode,
                               bool daemonize,
                               std::int64_t coin_port,
                               std::string&& coin_host,
                               std::string&& coin_user,
                               std::string&& coin_password,
                               std::int64_t rpc_port,
                               std::string&& rpc_user,
                               std::string&& rpc_password)
    : logfolder_(std::move(logfolder)),
      mode_(mode),
      daemonize_(daemonize),
      coin_port_(coin_port),
      coin_host_(std::move(coin_host)),
      coin_user_(std::move(coin_user)),
      coin_password_(std::move(coin_password)),
      rpc_port_(rpc_port),
      rpc_user_(std::move(rpc_user)),
      rpc_password_(std::move(rpc_password)) {}

auto ProgramOptions::getLogFolder() const
    -> const std::string&
{
    return logfolder_;
}


auto ProgramOptions::shouldLogToConsole() const
    -> bool
{
    return log_to_console_;
}

auto ProgramOptions::setShouldLogToConsole(bool value)
    -> bool
{
    log_to_console_ = value;
}


auto ProgramOptions::getMode() const
    -> Mode
{
    return mode_;
}

auto ProgramOptions::shouldDaemonize() const
    -> bool
{
    return daemonize_;
}

auto ProgramOptions::getCoinPort() const
    -> std::int64_t
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
    -> std::int64_t
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

        auto params = parseConfigFile(config_path);

        params.setShouldLogToConsole(log_to_console);

        return params;

    } catch(const std::exception& e) {
        fmt::print("{}\n", e.what());
        fmt::print("Something went wrong with the parameters or the config file\n");
        std::exit(-1);
    }
}

auto buddy::env::parseConfigFile(const std::string& config_path)
    -> ProgramOptions
{
    auto config = cpptoml::parse_file(config_path + "/buddy.conf");
    auto log_path = config->get_qualified_as<std::string>("log-folder").value_or(config_path + "/log/");
    auto mode_str = *config->get_qualified_as<std::string>("client.mode");
    auto daemonize = *config->get_qualified_as<bool>("client.daemon");
    auto coin_port = *config->get_qualified_as<std::int64_t>("coin.port");
    auto coin_host = *config->get_qualified_as<std::string>("coin.host");
    auto coin_user = *config->get_qualified_as<std::string>("coin.user");
    auto coin_password = *config->get_qualified_as<std::string>("coin.password");
    auto rpc_port = config->get_qualified_as<std::int64_t>("rpc.port").value_or(25000);
    auto rpc_user = config->get_qualified_as<std::string>("rpc.user").value_or("user");
    auto rpc_password = config->get_qualified_as<std::string>("rpc.password").value_or("password");

    auto mode = [&]() {
        if(mode_str == "lookup")
            return buddy::env::Mode::LookupOnly;
        else if(mode_str == "readonly")
            return buddy::env::Mode::ReadOnly;
        else if(mode_str == "readwrite")
            return buddy::env::Mode::ReadWrite;
        else {
            fmt::print("invalid value for \"client.mode\", should be \"lookup\", \"readonly\" or \"readwrite\"");
            std::exit(0);
        }
    }();

    return ProgramOptions{std::move(log_path),
                          mode,
                          daemonize,
                          coin_port,
                          std::move(coin_host),
                          std::move(coin_user),
                          std::move(coin_password),
                          rpc_port,
                          std::move(rpc_user),
                          std::move(rpc_password)};
}
