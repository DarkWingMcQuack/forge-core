#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/readwritewalletstubclient.h>

namespace forge::cli {

auto addShutdown(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addUpdateLookup(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addRebuildLookup(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addCheckValidity(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addGetLastValidBlockHeight(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupUMValue(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupUniqueValue(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupOwner(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupActivationBlock(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;
auto addLookupAllEntrysOf(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addLookupOnlySubcommands(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

} // namespace forge::cli
