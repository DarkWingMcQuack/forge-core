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
    ProgramOptions(utilxx::Opt<std::string>&& logfolder,
                   Mode mode,
                   std::int64_t coin_port,
                   std::string&& coin_host,
                   std::string&& coin_user,
                   std::string&& coin_password,
                   std::int64_t rpc_port,
                   std::string&& rpc_user,
                   std::string&& rpc_password);

    auto getLogFolder() const
        -> const utilxx::Opt<std::string>;

    auto getMode() const
        -> Mode;

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
    utilxx::Opt<std::string> logfolder_;

    Mode mode_;

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

} // namespace buddy::env
