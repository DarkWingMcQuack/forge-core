#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/LookupOnlySubcommands.hpp>
#include <rpc/jsonrpcstubclient.h>


auto forge::cli::addLookupOnlySubcommands(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app.add_subcommand("lookup",
                       "subcommand for handling all commands regarding the forged lookup");

    app.add_subcommand("umentry",
                       "subcommand for handling unique modifiable entrys");

    app.add_subcommand("uniqueentry",
                       "subcommand for handling unique unmodifiable entrys");
    app.add_subcommand("utilitytoken",
                       "subcommand for handling utility token");

    addShutdown(app, client);
    addUpdateLookup(app, client);
    addRebuildLookup(app, client);
    addCheckValidity(app, client);
    addGetLastValidBlockHeight(app, client);
    addLookupUMValue(app, client);
    addLookupUniqueValue(app, client);
    addLookupOwner(app, client);
    addLookupActivationBlock(app, client);
    addLookupAllEntrysOf(app, client);
    addGetUtilityTokenBalanceOf(app, client);
    addGetSupplyOfUtilityToken(app, client);
}

auto forge::cli::addShutdown(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app
        .add_subcommand("shutdown",
                        "stops the forged server")
        ->callback([&] {
            client.shutdown();
        });
}

auto forge::cli::addUpdateLookup(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app.get_subcommand("lookup")
        ->add_subcommand("update",
                         "updates the lookup to the newest block")
        ->callback([&] {
            RESPONSE = client.updatelookup();
        });
}

auto forge::cli::addRebuildLookup(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app.get_subcommand("lookup")
        ->add_subcommand("rebuild",
                         "rebuilds the lookup from the beginning")
        ->callback([&] {
            client.rebuildlookup();
        });
}

auto forge::cli::addCheckValidity(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app.get_subcommand("lookup")
        ->add_subcommand("checkvalidity",
                         "checks if the lookup is valid in terms of blockhashes")
        ->callback([&] {
            RESPONSE = client.checkvalidity();
        });
}

auto forge::cli::addGetLastValidBlockHeight(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    app.get_subcommand("lookup")
        ->add_subcommand("getlastvalidblockheight",
                         "return the blockheight until which the forge node is in sync with the coin node")
        ->callback([&] {
            RESPONSE = client.getlastvalidblockheight();
        });
}

auto forge::cli::addLookupUMValue(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    auto lookupumvalue_opt =
        app.get_subcommand("umentry")
            ->add_subcommand("lookupvalue",
                             "looks up the value of a given byte vector/string")
            ->callback([&] {
                RESPONSE = client.lookupumvalue(IS_STRING, KEY);
            });

    lookupumvalue_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    lookupumvalue_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto forge::cli::addLookupUniqueValue(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    auto lookupumvalue_opt =
        app.get_subcommand("uniqueentry")
            ->add_subcommand("lookupvalue",
                             "looks up the value of a given byte vector/string")
            ->callback([&] {
                RESPONSE = client.lookupuniquevalue(IS_STRING, KEY);
            });

    lookupumvalue_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    lookupumvalue_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto forge::cli::addLookupOwner(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
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
                     KEY,
                     "the key of which the owner will be looked up")
        ->required();

    lookupowner_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");
}

auto forge::cli::addLookupActivationBlock(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
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

auto forge::cli::addLookupAllEntrysOf(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    auto lookupallentrysof_opt =
        app.get_subcommand("lookup")
            ->add_subcommand("allentrysof",
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

auto forge::cli::addGetUtilityTokenBalanceOf(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    auto getbalance_opt =
        app.get_subcommand("utilitytoken")
            ->add_subcommand("getbalance",
                             "returns the balance of a user of a given utility token")
            ->callback([&] {
                RESPONSE = client.getbalanceof(IS_STRING, OWNER, KEY);
            });

    getbalance_opt
        ->add_option("--owner",
                     OWNER,
                     "the address of the owner")
        ->required();

    getbalance_opt
        ->add_option("--token-id",
                     KEY,
                     "the id of the utility token")
        ->required();

    getbalance_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given id will be interpreted as string and not as byte vector");
}

auto forge::cli::addGetSupplyOfUtilityToken(CLI::App& app, forge::rpc::JsonRpcStubClient& client)
    -> void
{
    auto supply_opt =
        app.get_subcommand("utilitytoken")
            ->add_subcommand("getsupply",
                             "returns the total supply of a utility token")
            ->callback([&] {
                RESPONSE = client.getsupplyofutilitytoken(IS_STRING, KEY);
            });

    supply_opt
        ->add_option("--token-id",
                     KEY,
                     "the id of the utility token")
        ->required();

    supply_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given token id will be interpreted as string and not as byte vector");
}
