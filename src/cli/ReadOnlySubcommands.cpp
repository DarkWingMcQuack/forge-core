#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/ReadOnlySubcommands.hpp>
#include <rpc/readwritewalletstubclient.h>


auto forge::cli::addReadOnlySubcommands(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void
{
    addAddWatchOnlyAddress(app, client);
    addDeleteWatchOnlyAddress(app, client);
    addAddNewOwnedAddress(app, client);
    addGetOwnedEntrys(app, client);
    addGetWatchedOnlyEntrys(app, client);
    addGetAllWatchedEntrys(app, client);
    addGetWatchedAddresses(app, client);
    addGetOwnedAddresses(app, client);
}

auto forge::cli::addAddWatchOnlyAddress(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("addwatchonlyaddress",
                       "adds a new address to watch")
        ->callback([&] {
            client.addwatchonlyaddress(OWNER);
        })
        ->add_option("--address", OWNER)
        ->required();
}

auto forge::cli::addDeleteWatchOnlyAddress(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("deletewatchonlyaddress",
                       "deletes an address from being watched")
        ->callback([&] {
            client.deletewatchonlyaddress(OWNER);
        })
        ->add_option("--address", OWNER)
        ->required();
}

auto forge::cli::addAddNewOwnedAddress(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("addnewownedaddress",
                       "adds a new address which the wallet ownes")
        ->callback([&] {
            client.addwatchonlyaddress(OWNER);
        })
        ->add_option("--address", OWNER)
        ->required();
}

auto forge::cli::addGetOwnedEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getownedentrys",
                       "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            RESPONSE = client.getownedentrys();
        });
}

auto forge::cli::addGetWatchedOnlyEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getwatchedonlyentrys",
                       "returns a list of all entrys the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchonlyentrys();
        });
}

auto forge::cli::addGetAllWatchedEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getallwatchedentrys",
                       "returns a list of all entrys the wallet observes")
        ->callback([&] {
            RESPONSE = client.getallwatchedentrys();
        });
}

auto forge::cli::addGetWatchedAddresses(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getwatchedaddresses",
                       "returns a list of all addresses the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchedaddresses();
        });
}

auto forge::cli::addGetOwnedAddresses(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getownedaddresses",
                       "returns a list of all addresses the wallet owns")
        ->callback([&] {
            RESPONSE = client.getownedaddresses();
        });
}

auto forge::cli::addOwnesAddress(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("ownesaddress",
                       "returns if the current wallet ownes the given address")
        ->callback([&] {
            RESPONSE = client.ownesaddress(OWNER);
        })
        ->add_option("--address", OWNER)
        ->required();
}
