#pragma once

#include <core/Coin.hpp>
#include <string>
#include <utilxx/Opt.hpp>


namespace forge::env {

constexpr static inline auto DEFAULT_CONFIG_FILE =
    "[server]\n"
    "mode = \"readwrite\"\n"
    "daemon = true\n"
    "threads = 10\n\n"

    "[rpc]\n"
    "user = \"\"\n"
    "password = \"\"\n"
    "port = 6969\n\n"

    "[coin]\n"
    "coin = \"odin\"\n"
    "user = \"user\"\n"
    "password = \"password\"\n"
    "host = \"localhost\"\n"
    "port = 22101\n";


enum class Mode {
    LookupOnly,
    ReadOnly,
    ReadWrite
};

class ProgramOptions
{
public:
    ProgramOptions(std::string&& logfolder,
                   std::int64_t number_of_threads,
                   Mode mode,
                   bool daemonize,
                   core::Coin coin,
                   std::int64_t coin_port,
                   std::string&& coin_host,
                   std::string&& coin_user,
                   std::string&& coin_password,
                   std::int64_t rpc_port,
                   std::string&& rpc_user,
                   std::string&& rpc_password);

    auto getLogFolder() const
        -> const std::string&;

    auto shouldLogToConsole() const
        -> bool;

    auto setShouldLogToConsole(bool)
        -> void;

    auto getMode() const
        -> Mode;
    auto shouldDaemonize() const
        -> bool;

    auto getCoin() const
        -> core::Coin;

    auto getNumberOfThreads() const
        -> std::int64_t;

    auto getCoinPort() const
        -> std::int64_t;
    auto getCoinHost() const
        -> const std::string&;
    auto getCoinUser() const
        -> const std::string&;
    auto getCoinPassword() const
        -> const std::string&;

    auto getRpcPort() const
        -> std::int64_t;
    auto getRpcUser() const
        -> const std::string&;
    auto getRpcPassword() const
        -> const std::string&;

private:
    std::string logfolder_;
    bool log_to_console_;

    std::int64_t number_of_threads_;

    Mode mode_;
    bool daemonize_;
    core::Coin coin_;

    std::int64_t coin_port_;
    std::string coin_host_;
    std::string coin_user_;
    std::string coin_password_;

    std::int64_t rpc_port_;
    std::string rpc_user_;
    std::string rpc_password_;
};

auto parseOptions(int argc, char* argv[])
    -> ProgramOptions;

auto parseConfigFile(const std::string& config_path)
    -> ProgramOptions;

} // namespace forge::env
