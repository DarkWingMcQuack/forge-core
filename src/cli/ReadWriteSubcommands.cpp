#include <CLI/CLI.hpp>
#include <cli/CLIGlobalVariables.hpp>
#include <cli/ReadWriteSubcommands.hpp>
#include <core/Transaction.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <fmt/core.h>
#include <rpc/readwritewalletstubclient.h>

using forge::core::stringToByteVec;


namespace {

auto parseIntoJson(const std::string& str)
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    std::string errors;

    reader->parse(
        str.c_str(),
        str.c_str() + str.size(),
        &json,
        &errors);
    delete reader;

    return json;
}

auto checkAndTransformValueString(const std::string& str)
    -> Json::Value
{
    auto json = parseIntoJson(str);

    if(!json.isMember("type")
       || !json["type"].isString()) {
        fmt::print("given json value {}, is invalid, the json object needs a string field with name \"type\"\n",
                   json.toStyledString());
        std::exit(0);
    }

    const auto& type = json["type"].asString();

    if(type != "ipv4"
       && type != "ipv6"
       && type != "bytearray"
       && type != "none") {

        fmt::print(
            "in value json object only the type \"ipv4\", \"ipv6\", "
            "\"bytearray\" or \"none\" are allowed, currently given {}\n",
            type);
        std::exit(0);
    }


    if(type == "ipv4") {
        const auto& value = json["value"].asString();
        if(value.size() != 8) {
            fmt::print("given ipv4 value has invalid length of {}, but should have 8\n",
                       value.size());
            std::exit(0);
        }

        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector\n",
                       value);
            std::exit(0);
        }

    } else if(type == "ipv6") {
        const auto& value = json["value"].asString();
        if(value.size() != 32) {
            fmt::print("given ipv6 value has invalid length of {}, but should have 8\n",
                       value.size());
            std::exit(0);
        }

        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector\n",
                       value);
            std::exit(0);
        }

    } else if(type == "bytevec") {
        const auto& value = json["value"].asString();
        if(!stringToByteVec(value)) {
            fmt::print("given value {} cannot be converted to byte vector",
                       value);
            std::exit(0);
        }
    } else if(type == "none"
              && json.isMember("value")) {

        if(!json["value"].isString()
           || !json["value"].asString().empty()) {
            fmt::print("since type of value is \"none\", given value {} will be ignored\n",
                       json["value"].toStyledString());
        }
    }

    return json;
}

} // namespace

auto forge::cli::addCreateUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto createnewentry_opt =
        app
            .get_subcommand("umentry")
            ->add_subcommand("create",
                             "creates a new entry")
            ->callback([&] {
                ENTRY_VALUE = checkAndTransformValueString(ENTRY_VALUE_STR);
                RESPONSE = client.createnewumentry(OWNER, BURN_VALUE, IS_STRING, KEY, ENTRY_VALUE);
            });

    createnewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--value",
                     ENTRY_VALUE_STR,
                     "value the entry holds, expected to be a json object "
                     "with valid \"type\" and \"value\" fields. "
                     "If not given, the entry will hold \"none\" as value");

    createnewentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    createnewentry_opt
        ->add_option("--address",
                     OWNER,
                     "optional address which will be used as owner of the new entry, "
                     "be sure the used wallet ownes the private key to this address. "
                     "If not given, then a new address will be generated");
}

auto forge::cli::addCreateUniqueEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto createnewentry_opt =
        app
            .get_subcommand("uniqueentry")
            ->add_subcommand("create",
                             "creates a new uniqueentry")
            ->callback([&] {
                ENTRY_VALUE = checkAndTransformValueString(ENTRY_VALUE_STR);
                RESPONSE = client.createnewuniqueentry(OWNER, BURN_VALUE, IS_STRING, KEY, ENTRY_VALUE);
            });

    createnewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--value",
                     ENTRY_VALUE_STR,
                     "value the entry holds, expected to be a json object "
                     "with valid \"type\" and \"value\" fields. "
                     "If not given, the entry will hold \"none\" as value");

    createnewentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    createnewentry_opt
        ->add_option("--address",
                     OWNER,
                     "optional address which will be used as owner of the new entry, "
                     "be sure the used wallet ownes the private key to this address. "
                     "If not given, then a new address will be generated");
}

auto forge::cli::addRenewEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto renewentry_opt =
        app.get_subcommand("entry")
            ->add_subcommand("renew",
                             "renews the given entry for another period")
            ->callback([&] {
                RESPONSE = client.renewentry(BURN_VALUE, IS_STRING, KEY);
            });

    renewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of entry which will be renewed")
        ->required();

    renewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    renewentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned to perform the renewal")
        ->required();
}

auto forge::cli::addDeleteEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto deleteentry_opt =
        app.get_subcommand("entry")
            ->add_subcommand("delete",
                             "deletes the given entry for another period")
            ->callback([&] {
                RESPONSE = client.deleteentry(BURN_VALUE, IS_STRING, KEY);
            });

    deleteentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of entry which will be deleted")
        ->required();

    deleteentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    deleteentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned to perform the deletion")
        ->required();
}

auto forge::cli::addUpdateUMEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto updateentry_opt =
        app
            .get_subcommand("umentry")
            ->add_subcommand("updateentry",
                             "updates an entry to map to a new value")
            ->callback([&] {
                ENTRY_VALUE = checkAndTransformValueString(ENTRY_VALUE_STR);
                RESPONSE = client.updateumentry(BURN_VALUE, IS_STRING, KEY, ENTRY_VALUE);
            });

    updateentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of the entry which will be updated")
        ->required();

    updateentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    updateentry_opt
        ->add_option("--value",
                     ENTRY_VALUE_STR,
                     "value the entry holds, expected to be a json object "
                     "with valid \"type\" and \"value\" fields. "
                     "If not given, the entry will hold \"none\" as value");

    updateentry_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();
}

auto forge::cli::addTransferOwnership(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto transferownership_opt =
        app.get_subcommand("entry")
            ->add_subcommand("transferownership",
                             "updates an entry to map to a new value")
            ->callback([&] {
                ENTRY_VALUE = checkAndTransformValueString(ENTRY_VALUE_STR);
                RESPONSE = client.transferownership(BURN_VALUE, IS_STRING, KEY, OWNER);
            });

    transferownership_opt
        ->add_option("--key",
                     KEY,
                     "the key of the entry which will be updated")
        ->required();

    transferownership_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    transferownership_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    transferownership_opt
        ->add_option("--new-owner",
                     OWNER,
                     "the address which will be the owner after the transfer is performed")
        ->required();
}

auto forge::cli::addPayToEntry(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto createnewentry_opt =
        app.add_subcommand("paytoentry",
                           "sends a given number of coins to the owner of a given entry")
            ->callback([&] {
                RESPONSE = client.paytoentryowner(BURN_VALUE, IS_STRING, KEY);
            });

    createnewentry_opt
        ->add_option("--key",
                     KEY,
                     "the key of which the value will be looked up")
        ->required();

    createnewentry_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given key will be interpreted as string and not as byte vector");

    createnewentry_opt
        ->add_option("--amount",
                     BURN_VALUE,
                     "number of coins which will be send")
        ->required();
}

auto forge::cli::addTransferUtilityToken(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto transferownership_opt =
        app.get_subcommand("utilitytoken")
            ->add_subcommand("transfer",
                             "transfers a given amount of tokens to a new owner")
            ->callback([&] {
                RESPONSE = client.sendutilitytokens(AMOUNT,
                                                    BURN_VALUE,
                                                    IS_STRING,
                                                    KEY,
                                                    OWNER);
            });

    transferownership_opt
        ->add_option("--token-id",
                     KEY,
                     "the token-id of the token which will be send")
        ->required();

    transferownership_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given token-id will be interpreted as string and not as byte vector");

    transferownership_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    transferownership_opt
        ->add_option("--amount",
                     AMOUNT,
                     "number of utility tokens which will be send")
        ->required();

    transferownership_opt
        ->add_option("--new-owner",
                     OWNER,
                     "the address to which the utility tokens will be send")
        ->required();
}

auto forge::cli::addDeleteUtilityToken(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto delete_opt =
        app.get_subcommand("utilitytoken")
            ->add_subcommand("burn",
                             "burns a given amount of tokens")
            ->callback([&] {
                RESPONSE = client.burnutilitytokens(AMOUNT, BURN_VALUE, IS_STRING, KEY);
            });

    delete_opt
        ->add_option("--token-id",
                     KEY,
                     "the token-id of the token which will be burned")
        ->required();

    delete_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given token-id will be interpreted as string and not as byte vector");

    delete_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    delete_opt
        ->add_option("--amount",
                     AMOUNT,
                     "number of utility tokens which will be send")
        ->required();
}

auto forge::cli::addCreateUtilityToken(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    auto create_opt =
        app.get_subcommand("utilitytoken")
            ->add_subcommand("create",
                             "creates a new type of utility token")
            ->callback([&] {
                RESPONSE = client.createnewutilitytoken(OWNER, BURN_VALUE, IS_STRING, KEY, AMOUNT);
            });

    create_opt
        ->add_option("--token-id",
                     KEY,
                     "the token-id of the token which will be burned")
        ->required();

    create_opt
        ->add_flag("--isstring",
                   IS_STRING,
                   "if set, the given token-id will be interpreted as string and not as byte vector");

    create_opt
        ->add_option("--burn-value",
                     BURN_VALUE,
                     "number of coins which will be burned")
        ->required();

    create_opt
        ->add_option("--amount",
                     AMOUNT,
                     "number of utility tokens which will be send")
        ->required();

    create_opt
        ->add_option("--address",
                     OWNER,
                     "addres which will be the owner of all the newly created tokens, if not set a new address will be generated");
}

auto forge::cli::addReadWriteSubcommands(CLI::App& app, forge::rpc::ReadWriteWalletStubClient& client)
    -> void
{
    app.add_subcommand("entry",
                       "subcommand to handle all operations "
                       "which can be executed independend of the type of the entry");

    addCreateUMEntry(app, client);
    addUpdateUMEntry(app, client);
    addCreateUniqueEntry(app, client);
    addRenewEntry(app, client);
    addDeleteEntry(app, client);
    addTransferOwnership(app, client);
    addPayToEntry(app, client);
    addTransferUtilityToken(app, client);
    addDeleteUtilityToken(app, client);
    addCreateUtilityToken(app, client);
}
