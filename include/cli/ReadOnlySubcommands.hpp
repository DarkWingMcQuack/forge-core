#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/readwritewalletstubclient.h>

namespace forge::cli {

auto addAddWatchOnlyAddress(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addDeleteWatchOnlyAddress(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addAddNewOwnedAddress(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetWatchedAddresses(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetOwnedAddresses(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addOwnesAddress(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetOwnedUMEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetWatchedOnlyUMEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetAllWatchedUMEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetOwnedUniqueEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetWatchedOnlyUniqueEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetAllWatchedUniqueEntrys(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addReadOnlySubcommands(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void;
} // namespace forge::cli
