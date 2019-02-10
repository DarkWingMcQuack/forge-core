#pragma once

#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <rpc/readwritewalletstubclient.h>

namespace forge::cli {

auto addCreateUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addRenewUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addUpdateUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addDeleteUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addTransferOwnership(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addPayToUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;

auto addReadWriteSubcommands(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void;


} // namespace forge::cli
