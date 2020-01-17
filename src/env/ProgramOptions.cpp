#include <CLI/CLI.hpp>
#include <CLI/Validators.hpp>
#include <cpptoml.h>
#include <env/ProgramOptions.hpp>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <g3log/g3log.hpp>
#include <string>
#include <utilxx/Opt.hpp>

using forge::env::ProgramOptions;

ProgramOptions::ProgramOptions(std::string&& logfolder,
                               std::int64_t number_of_threads,
                               Mode mode,
                               bool clientize,
                               core::Coin coin,
                               std::int64_t coin_port,
                               std::string&& coin_host,
                               std::string&& coin_user,
                               std::string&& coin_password,
                               std::int64_t rpc_port,
                               std::string&& rpc_user,
                               std::string&& rpc_password)
    : logfolder_(std::move(logfolder)),
      number_of_threads_(number_of_threads),
      mode_(mode),
      clientize_(clientize),
      coin_(coin),
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
    -> void
{
    log_to_console_ = value;
}


auto ProgramOptions::getMode() const
    -> Mode
{
    return mode_;
}

auto ProgramOptions::getCoin() const
    -> core::Coin
{
    return coin_;
}

auto ProgramOptions::shouldClientize() const
    -> bool
{
    return clientize_;
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

auto ProgramOptions::getNumberOfThreads() const
    -> std::int64_t
{
    return number_of_threads_;
}

namespace {

auto getBasePathFromEnv()
    -> std::string
{
    using namespace std::string_literals;

    auto raw_str = std::getenv("BASE_DIR");
    if(raw_str == nullptr) {
        return getenv("HOME") + "/.forge"s;
    }

    return raw_str;
}

auto getServerModeFromEnv()
    -> std::string
{
    auto raw_str = std::getenv("SERVER_MODE");
    if(raw_str == nullptr) {
        return "readwrite";
    }

    return raw_str;
}

auto getClientModeFromEnv()
    -> bool
{
    auto raw_str = std::getenv("DEBUG");
    std::string str = raw_str ? raw_str : "";
    for(auto& ch : str) {
        ch = std::toupper(ch);
    }

    return str == "TRUE";
}

auto getCoinStringFromEnv()
    -> std::string
{
    auto raw_str = std::getenv("COIN");
    if(raw_str == nullptr) {
        return "odin";
    }

    return raw_str;
}

auto getCoinPortEnv()
{
    try {
        auto raw_str = std::getenv("COIN_PORT");
        return std::stoi(raw_str);
    } catch(...) {
        return 22101;
    }
}

auto getCoinHostFromEnv()
    -> std::string
{
    auto raw_str = std::getenv("COIN_HOST");
    if(raw_str == nullptr) {
        return "localhost";
    }

    return raw_str;
}

auto getCoinUserFromEnv()
    -> std::string
{
    auto raw_str = std::getenv("COIN_USER");
    if(raw_str == nullptr) {
        return "user";
    }

    return raw_str;
}

auto getCoinPasswordFromEnv()
    -> std::string
{
    auto raw_str = std::getenv("COIN_PASSWORD");
    if(raw_str == nullptr) {
        return "password";
    }

    return raw_str;
}

auto getRPCPortEnv()
{
    try {
        auto raw_str = std::getenv("RPC_PORT");
        return std::stoi(raw_str);
    } catch(...) {
        return 6969;
    }
}

auto getThreadsEnv()
{
    try {
        auto raw_str = std::getenv("THREADS");
        return std::stoi(raw_str);
    } catch(...) {
        return 10;
    }
}

} // namespace

auto forge::env::parseOptions(int argc, char* argv[])
    -> ProgramOptions
{
    using namespace std::string_literals;
    namespace fs = std::filesystem;

    static const auto default_forge_dir = getenv("HOME") + "/.forge"s;

    CLI::App app{"forged server, adding a second layer to blockchains"};

    bool log_to_console = false;
    bool read_config_from_env = false;
    auto config_path = default_forge_dir;
    app.set_help_all_flag("--help-all",
                          "Show all help");

    app.add_flag("-l,--log",
                 log_to_console,
                 "if set, logging will be displayed on the terminal");

    app.add_flag("-e,--env",
                 read_config_from_env,
                 "if set, logging will be displayed on the terminal");

    app.add_option("-w,--workdir",
                   config_path,
                   "path to the workingfolder of forge")
        ->check(CLI::ExistingPath);
    try {
        app.parse(argc, argv);
    } catch(const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    if(read_config_from_env) {
        config_path = getBasePathFromEnv();
        fs::create_directory(config_path);


        auto params = fromEnvironment();

        params.setShouldLogToConsole(log_to_console);

        return params;
    }


    if(!fs::exists(config_path + "/forge.conf")) {
        fmt::print("config file {} not found\n",
                   config_path + "/forge.conf");

        std::ofstream ofs{config_path + "/forge.conf"};

        ofs << DEFAULT_CONFIG_FILE;

        fmt::print("created default config file in workdir {}\n",
                   config_path);
    }

    auto params = parseConfigFile(config_path);

    params.setShouldLogToConsole(log_to_console);

    return params;
}

auto forge::env::parseConfigFile(const std::string& config_path)
    -> ProgramOptions
{
    namespace fs = std::filesystem;

    auto config = cpptoml::parse_file(config_path + "/forge.conf");
    auto log_path = config->get_qualified_as<std::string>("log-folder").value_or(config_path + "/log/");
    auto mode_str = *config->get_qualified_as<std::string>("server.mode");
    auto clientize = *config->get_qualified_as<bool>("server.client");
    auto coin_str = *config->get_qualified_as<std::string>("coin.coin");
    auto coin_port = *config->get_qualified_as<std::int64_t>("coin.port");
    auto coin_host = *config->get_qualified_as<std::string>("coin.host");
    auto coin_user = *config->get_qualified_as<std::string>("coin.user");
    auto coin_password = *config->get_qualified_as<std::string>("coin.password");
    auto rpc_port = config->get_qualified_as<std::int64_t>("rpc.port").value_or(25000);
    auto rpc_user = config->get_qualified_as<std::string>("rpc.user").value_or("user");
    auto rpc_password = config->get_qualified_as<std::string>("rpc.password").value_or("password");
    auto threads = config->get_qualified_as<std::int64_t>("server.threads").value_or(5);


    //create the log folder
    fs::create_directory(log_path);

    //try to get the mode
    auto mode = [&]() {
        if(mode_str == "lookup")
            return forge::env::Mode::LookupOnly;
        if(mode_str == "readonly")
            return forge::env::Mode::ReadOnly;
        if(mode_str == "readwrite")
            return forge::env::Mode::ReadWrite;
        else {
            fmt::print(R"(invalid value for "server.mode", should be "lookup", "readonly" or "readwrite")");
            std::exit(-1);
        }
    }();

    //try to get the coind
    auto coin_opt = core::fromString(coin_str);
    if(!coin_opt) {
        fmt::print("invalid value for \"coin.coin\" = {}", coin_str);
        std::exit(-1);
    }

    return ProgramOptions{std::move(log_path),
                          threads,
                          mode,
                          clientize,
                          coin_opt.getValue(),
                          coin_port,
                          std::move(coin_host),
                          std::move(coin_user),
                          std::move(coin_password),
                          rpc_port,
                          std::move(rpc_user),
                          std::move(rpc_password)};
}


auto forge::env::fromEnvironment()
    -> ProgramOptions
{
    namespace fs = std::filesystem;

    auto config = getBasePathFromEnv();
    auto log_path = config + "/logs/";
    auto mode_str = getServerModeFromEnv();
    auto clientize = getClientModeFromEnv();
    auto coin_str = getCoinStringFromEnv();
    auto coin_port = getCoinPortEnv();
    auto coin_host = getCoinHostFromEnv();
    auto coin_user = getCoinUserFromEnv();
    auto coin_password = getCoinPasswordFromEnv();
    auto rpc_port = getRPCPortEnv();
    auto rpc_user = "";
    auto rpc_password = "";
    auto threads = getThreadsEnv();

    //create the log folder
    fs::create_directory(log_path);

    //try to get the mode
    auto mode = [&]() {
        if(mode_str == "lookup") {
            return forge::env::Mode::LookupOnly;
        }
        if(mode_str == "readonly") {
            return forge::env::Mode::ReadOnly;
        }
        if(mode_str == "readwrite") {
            return forge::env::Mode::ReadWrite;
        }
        fmt::print(R"(invalid value for "server.mode", should be "lookup", "readonly" or "readwrite")");
        std::exit(-1);
    }();

    //try to get the coind
    auto coin_opt = core::fromString(coin_str);
    if(!coin_opt) {
        fmt::print("invalid value for \"coin.coin\" = {}", coin_str);
        std::exit(-1);
    }

    return ProgramOptions{std::move(log_path),
                          threads,
                          mode,
                          clientize,
                          coin_opt.getValue(),
                          coin_port,
                          std::move(coin_host),
                          std::move(coin_user),
                          std::move(coin_password),
                          rpc_port,
                          std::move(rpc_user),
                          std::move(rpc_password)};
}
