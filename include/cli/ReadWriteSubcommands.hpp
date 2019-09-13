#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/jsonrpcstubclient.h>

namespace forge::cli {

auto addCreateUMEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addUpdateUMEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addCreateUniqueEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addRenewEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addDeleteEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addTransferOwnership(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addTransferUtilityToken(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addDeleteUtilityToken(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addCreateUtilityToken(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addPayToEntry(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addReadWriteSubcommands(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;


} // namespace forge::cli
