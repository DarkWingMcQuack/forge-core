#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/jsonrpcstubclient.h>

namespace forge::cli {

auto addShutdown(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addUpdateLookup(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addRebuildLookup(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addCheckValidity(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addGetLastValidBlockHeight(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addLookupUMValue(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addLookupUniqueValue(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addLookupOwner(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addLookupActivationBlock(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addLookupAllEntrysOf(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addGetUtilityTokenBalanceOf(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;
auto addGetSupplyOfUtilityToken(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

auto addLookupOnlySubcommands(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void;

} // namespace forge::cli
