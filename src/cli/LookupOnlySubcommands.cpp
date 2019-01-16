#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/LookupOnlySubcommands.hpp>
#include <rpc/readwritewalletstubclient.h>


auto buddy::cli::addLookupOnlySubcommands(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    addShutdown(app, client);
    addUpdateLookup(app, client);
    addRebuildLookup(app, client);
    addCheckValidity(app, client);
    addLookupValue(app, client);
    addLookupOwner(app, client);
    addLookupActivationBlock(app, client);
    addLookupAllEntrysOf(app, client);
}

auto buddy::cli::addShutdown(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("shutdown",
                       "stops the buddyd server")
        ->callback([&] {
            client.shutdown();
        });
}

auto buddy::cli::addUpdateLookup(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("updatelookup",
                       "updates the lookup to the newest block")
        ->callback([&] {
            RESPONSE = client.updatelookup();
        });
}

auto buddy::cli::addRebuildLookup(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("rebuildlookup",
                       "rebuilds the lookup from the beginning")
        ->callback([&] {
            client.rebuildlookup();
        });
}

auto buddy::cli::addCheckValidity(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("checkvalidity",
                       "checks if the lookup is valid in terms of blockhashes")
        ->callback([&] {
            RESPONSE = client.checkvalidity();
        });
}

auto buddy::cli::addLookupValue(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto lookupvalue_opt =
        app.add_subcommand("lookupvalue",
                           "looks up the value of a given byte vector/string")
            ->callback([&] {
                RESPONSE = client.lookupvalue(IS_STRING, KEY);
            });

    lookupvalue_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    lookupvalue_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto buddy::cli::addLookupOwner(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto lookupowner_opt =
        app.add_subcommand("lookupowner",
                           "looks up the owner of an entry identified by a given byte vector/string key")
            ->callback([&] {
                RESPONSE = client.lookupowner(IS_STRING, KEY);
            });

    lookupowner_opt
        ->add_option("--key",
                     IS_STRING,
                     "the key of which the owner will be looked up")
        ->required();

    lookupowner_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto buddy::cli::addLookupActivationBlock(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto lookupactivationblock_opt =
        app.add_subcommand("lookupactivationblock",
                           "looks up the in which block the entry with the given key was last activated")
            ->callback([&] {
                RESPONSE = client.lookupactivationblock(IS_STRING, KEY);
            });

    lookupactivationblock_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    lookupactivationblock_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto buddy::cli::addLookupAllEntrysOf(CLI::App& app, buddy::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto lookupallentrysof_opt =
        app.add_subcommand("lookupallentrysof",
                           "returns a vector of all entrys the given owner currently owns")
            ->callback([&] {
                RESPONSE = client.lookupallentrysof(OWNER);
            });

    lookupallentrysof_opt
        ->add_option("--owner",
                     OWNER,
                     "owner address of which all the entrys will be looked up")
        ->required();
}
