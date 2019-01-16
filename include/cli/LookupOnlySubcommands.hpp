#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/readwritewalletstubclient.h>

namespace buddy::cli {

auto addShutdown(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addUpdateLookup(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addRebuildLookup(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addCheckValidity(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupValue(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupOwner(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupActivationBlock(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupAllEntrysOf(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addLookupOnlySubcommands(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void;

} // namespace buddy::cli
