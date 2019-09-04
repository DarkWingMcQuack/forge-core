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
    addGetOwnedUMEntrys(app, client);
    addGetWatchedOnlyUMEntrys(app, client);
    addGetAllWatchedUMEntrys(app, client);
    addGetOwnedUniqueEntrys(app, client);
    addGetWatchedOnlyUniqueEntrys(app, client);
    addGetAllWatchedUniqueEntrys(app, client);
    addGetWatchedAddresses(app, client);
    addGetOwnedAddresses(app, client);
    addGetOwnedUtilityTokens(app, client);
    addGetWatchedOnlyUtilityTokens(app, client);
    addGetAllWatchedUtilityTokens(app, client);
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

auto forge::cli::addGetOwnedUMEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.get_subcommand("umentry")
        ->add_subcommand("getownedentrys",
                         "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            RESPONSE = client.getownedumentrys();
        });
}

auto forge::cli::addGetWatchedOnlyUMEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.get_subcommand("umentry")
        ->add_subcommand("getwatchedonlyentrys",
                         "returns a list of all entrys the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchonlyumentrys();
        });
}

auto forge::cli::addGetAllWatchedUMEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app
        .get_subcommand("umentry")
        ->add_subcommand("getallwatchedentrys",
                         "returns a list of all entrys the wallet observes")
        ->callback([&] {
            RESPONSE = client.getallwatchedumentrys();
        });
}

auto forge::cli::addGetOwnedUniqueEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.get_subcommand("uniqueentry")
        ->add_subcommand("getownedentrys",
                         "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            RESPONSE = client.getowneduniqueentrys();
        });
}

auto forge::cli::addGetWatchedOnlyUniqueEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.get_subcommand("uniqueentry")
        ->add_subcommand("getwatchedonlyentrys",
                         "returns a list of all entrys the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getwatchonlyuniqueentrys();
        });
}

auto forge::cli::addGetAllWatchedUniqueEntrys(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app
        .get_subcommand("uniqueentry")
        ->add_subcommand("getallwatchedentrys",
                         "returns a list of all entrys the wallet observes")
        ->callback([&] {
            RESPONSE = client.getallwatcheduniqueentrys();
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


auto forge::cli::addGetOwnedUtilityTokens(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app
        .get_subcommand("utilitytoken")
        ->add_subcommand("getallwatchedtokens",
                         "returns a list of all utility tokens the wallet observes")
        ->callback([&] {
            RESPONSE = client.getallwatchedutilitytokens();
        });
}
auto forge::cli::addGetWatchedOnlyUtilityTokens(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app
        .get_subcommand("utilitytoken")
        ->add_subcommand("getallwatchedonlytokens",
                         "returns a list of all tokens the wallet observes but does not own")
        ->callback([&] {
            RESPONSE = client.getallwatchedutilitytokens();
        });
}
auto forge::cli::addGetAllWatchedUtilityTokens(CLI::App& app, rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.get_subcommand("utilitytoken")
        ->add_subcommand("getallownedtokens",
                         "returns a list of all entrys the wallet ownes")
        ->callback([&] {
            RESPONSE = client.getownedutilitytokens();
        });
}
