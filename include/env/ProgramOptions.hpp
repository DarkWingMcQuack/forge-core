#pragma once

#include <string>
#include <utilxx/Opt.hpp>


namespace buddy::env {

enum class Mode {
    LookupOnly,
    ReadOnly,
    ReadWrite
};

auto operator>>(std::istream& in, Mode& mode)
    -> std::istream&;
auto operator<<(std::ostream& os, const Mode& mode)
    -> std::ostream&;

class ProgramOptions
{
public:
    ProgramOptions(std::string&& logfolder,
                   std::int64_t number_of_threads,
                   Mode mode,
                   bool daemonize,
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
        -> bool;

    auto getMode() const
        -> Mode;
    auto shouldDaemonize() const
        -> bool;

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

} // namespace buddy::env
