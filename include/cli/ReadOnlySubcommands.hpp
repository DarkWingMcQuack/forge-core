#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/jsonrpcstubclient.h>

namespace forge::cli {

auto addAddWatchOnlyAddress(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addDeleteWatchOnlyAddress(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addAddNewOwnedAddress(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetWatchedAddresses(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetOwnedAddresses(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addOwnesAddress(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetOwnedUMEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetWatchedOnlyUMEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetAllWatchedUMEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetOwnedUniqueEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetWatchedOnlyUniqueEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetAllWatchedUniqueEntrys(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetOwnedUtilityTokens(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetWatchedOnlyUtilityTokens(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
auto addGetAllWatchedUtilityTokens(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;

auto addReadOnlySubcommands(CLI::App& app, rpc::JsonRpcStubClient& client)
    -> void;
} // namespace forge::cli
