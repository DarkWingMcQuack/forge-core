#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/ReadOnlySubcommands.hpp>
#include <rpc/readwritewalletstubclient.h>


auto buddy::cli::addReadOnlySubcommands(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
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

auto buddy::cli::addAddWatchOnlyAddress(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
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

auto buddy::cli::addDeleteWatchOnlyAddress(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
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

auto buddy::cli::addAddNewOwnedAddress(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
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

auto buddy::cli::addGetOwnedEntrys(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getownedentrys",
                       "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            RESPONSE = client.getownedentrys();
        });
}

auto buddy::cli::addGetWatchedOnlyEntrys(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getwatchedonlyentrys",
                       "returns a list of all entrys the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchonlyentrys();
        });
}

auto buddy::cli::addGetAllWatchedEntrys(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getallwatchedentrys",
                       "returns a list of all entrys the wallet observes")
        ->callback([&] {
            RESPONSE = client.getallwatchedentrys();
        });
}

auto buddy::cli::addGetWatchedAddresses(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getwatchedaddresses",
                       "returns a list of all addresses the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchedaddresses();
        });
}

auto buddy::cli::addGetOwnedAddresses(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("getownedaddresses",
                       "returns a list of all addresses the wallet owns")
        ->callback([&] {
            RESPONSE = client.getownedaddresses();
        });
}
