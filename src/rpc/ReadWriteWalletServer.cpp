#include "core/Transaction.hpp"
#include "entrys/token/UtilityToken.hpp"
#include <chrono>
#include <fmt/core.h>
#include <fmt/format.h>
#include <g3log/g3log.hpp>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <numeric>
#include <rpc/ReadWriteWalletServer.hpp>
#include <thread>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Overload.hpp>
#include <variant>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>

using forge::rpc::ReadWriteWalletServer;
using forge::core::getBlockTimeInSeconds;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::IPv4Value;
using forge::core::IPv6Value;
using forge::core::ByteArray;
using forge::wallet::ReadWriteWallet;
using forge::wallet::ReadOnlyWallet;
using forge::lookup::LookupManager;
using jsonrpc::JsonRpcException;

ReadWriteWalletServer::ReadWriteWalletServer(jsonrpc::AbstractServerConnector& connector,
                                             jsonrpc::serverVersion_t type,
                                             wallet::ReadWriteWallet&& wallet)
    : AbstractReadWriteWalletStubSever(connector, type),
      logic_(std::move(wallet))
{
    startUpdaterThread();
}

ReadWriteWalletServer::ReadWriteWalletServer(jsonrpc::AbstractServerConnector& connector,
                                             jsonrpc::serverVersion_t type,
                                             wallet::ReadOnlyWallet&& wallet)
    : AbstractReadWriteWalletStubSever(connector, type),
      logic_(std::move(wallet))
{
    startUpdaterThread();
}

ReadWriteWalletServer::ReadWriteWalletServer(jsonrpc::AbstractServerConnector& connector,
                                             jsonrpc::serverVersion_t type,
                                             lookup::LookupManager&& lookup)
    : AbstractReadWriteWalletStubSever(connector, type),
      logic_(std::move(lookup))
{
    startUpdaterThread();
}


auto ReadWriteWalletServer::updatelookup()
    -> bool
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is already indexing"};
    }

    auto& lookup = getLookup();

    indexing_.store(true);

    auto res = lookup.updateLookup();

    indexing_.store(false);

    if(!res) {
        auto error_msg = lookup::generateMessage(std::move(res.getError()));
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::rebuildlookup()
    -> void
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is already indexing"};
    }

    auto& lookup = getLookup();

    indexing_.store(true);
    auto res = lookup.rebuildLookup();
    indexing_.store(false);

    if(!res) {
        auto error_msg = lookup::generateMessage(std::move(res.getError()));
        throw JsonRpcException{std::move(error_msg)};
    }
}

auto ReadWriteWalletServer::shutdown()
    -> void
{
    should_shutdown_.store(true);
}

auto ReadWriteWalletServer::lookupumvalue(bool isstring, const std::string& key)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, key);

    auto res = lookup.lookupUMValue(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return forge::core::umentryValueToJson(res.getValue().get());
}

auto ReadWriteWalletServer::lookupuniquevalue(bool isstring, const std::string& key)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, key);

    auto res = lookup.lookupUniqueValue(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return forge::core::umentryValueToJson(res.getValue().get());
}

auto ReadWriteWalletServer::lookupowner(bool isstring, const std::string& key)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, key);

    auto res = lookup.lookupOwner(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::lookupactivationblock(bool isstring, const std::string& key)
    -> int
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, key);

    auto res = lookup.lookupActivationBlock(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::checkvalidity()
    -> bool
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }
    auto& lookup = getLookup();

    auto res = lookup.lookupIsValid();

    if(!res) {
        auto error_msg = lookup::generateMessage(std::move(res.getError()));
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::getlastvalidblockheight()
    -> int
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto res = lookup.getLastValidBlockHeight();

    if(!res) {
        auto error_msg = lookup::generateMessage(std::move(res.getError()));
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::lookupallentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto entrys = lookup.getUMEntrysOfOwner(owner);

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::addwatchonlyaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    auto copy = address;
    wallet.addWatchOnlyAddress(std::move(copy));
}

auto ReadWriteWalletServer::deletewatchonlyaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    wallet.deleteWatchOnlyAddress(address);
}

auto ReadWriteWalletServer::addnewownedaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    auto copy = address;
    wallet.addNewOwnedAddress(std::move(copy));
}

auto ReadWriteWalletServer::getownedumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getOwnedUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getwatchonlyumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getWatchOnlyUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getallwatchedumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getAllWatchedUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getowneduniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getOwnedUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getwatchonlyuniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getWatchOnlyUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getallwatcheduniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getAllWatchedUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getwatchedaddresses()
    -> Json::Value
{
    const auto& wallet = getReadOnlyWallet();
    const auto& addresses = wallet.getWatchedAddresses();
    auto ret_json =
        std::accumulate(std::begin(addresses),
                        std::end(addresses),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, const auto& entry) {
                            init.append(entry);
                            return init;
                        });

    return ret_json;
}
auto ReadWriteWalletServer::getownedaddresses()
    -> Json::Value
{
    const auto& wallet = getReadOnlyWallet();
    const auto& addresses = wallet.getOwnedAddresses();
    auto ret_json =
        std::accumulate(std::begin(addresses),
                        std::end(addresses),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, const auto& entry) {
                            init.append(entry);
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::ownesaddress(const std::string& address)
    -> bool
{
    const auto& wallet = getReadOnlyWallet();
    return wallet.ownesAddress(address);
}

auto ReadWriteWalletServer::createnewumentry(const std::string& address,
                                             int burnvalue,
                                             bool is_string,
                                             const std::string& key,
                                             const Json::Value& value)
    -> std::string
{
    auto& wallet = getReadWriteWallet();
    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUMEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto entry_value = std::move(entry_value_opt.getValue());
    auto key_vec = extractEntryKey(is_string, key);

    if(address.empty()) {
        auto res = wallet.createNewUMEntry(std::move(key_vec),
                                           std::move(entry_value),
                                           burnvalue);


        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();
    }
    auto address_copy = address;
    auto res = wallet.createNewUMEntry(std::move(key_vec),
                                       std::move(entry_value),
                                       std::move(address_copy),
                                       burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}


auto ReadWriteWalletServer::updateumentry(int burnvalue,
                                          bool is_string,
                                          const std::string& key,
                                          const Json::Value& value)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUMEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto entry_value = std::move(entry_value_opt.getValue());
    auto key_vec = extractEntryKey(is_string, key);

    auto res = wallet.updateUMEntry(std::move(key_vec),
                                    std::move(entry_value),
                                    burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}


auto ReadWriteWalletServer::createnewuniqueentry(const std::string& address,
                                                 int burnvalue,
                                                 bool is_string,
                                                 const std::string& key,
                                                 const Json::Value& value)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUniqueEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto key_vec = extractEntryKey(is_string, key);
    auto entry_value = std::move(entry_value_opt.getValue());

    if(address.empty()) {
        auto res = wallet.createNewUniqueEntry(std::move(key_vec),
                                               std::move(entry_value),
                                               burnvalue);


        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();
    }
    auto address_copy = address;
    auto res = wallet.createNewUniqueEntry(std::move(key_vec),
                                           std::move(entry_value),
                                           std::move(address_copy),
                                           burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::renewentry(int burnvalue,
                                       bool is_string,
                                       const std::string& key)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto key_vec = extractEntryKey(is_string, key);
    auto res = wallet.renewEntry(std::move(key_vec),
                                 burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::deleteentry(int burnvalue,
                                        bool is_string,
                                        const std::string& key)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto key_vec = extractEntryKey(is_string, key);

    auto res = wallet.deleteEntry(std::move(key_vec),
                                  burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::transferownership(int burnvalue,
                                              bool is_string,
                                              const std::string& key,
                                              const std::string& newowner)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto key_vec = extractEntryKey(is_string, key);

    auto newowner_copy = newowner;

    auto res = wallet.transferOwnership(std::move(key_vec),
                                        std::move(newowner_copy),
                                        burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::paytoentryowner(int amount,
                                            bool is_string,
                                            const std::string& key)
    -> std::string
{
    auto& wallet = getReadWriteWallet();

    auto key_vec = extractEntryKey(is_string, key);

    auto res = wallet.payToEntryOwner(std::move(key_vec),
                                      amount);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::getownedutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();

    auto entrys = wallet.getOwnedUtilityTokens();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getwatchonlyutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();

    auto entrys = wallet.getWatchOnlyUtilityTokens();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getallwatchedutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();
    auto entrys = wallet.getAllWatchedUtilityTokens();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto init, auto entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getutilitytokensof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& lookup = getLookup();
    auto tokens = lookup.getUtilityTokensOfOwner(owner);

    auto json_entrys =
        utilxx::transform_into_vector(
            std::make_move_iterator(std::begin(tokens)),
            std::make_move_iterator(std::end(tokens)),
            [](auto entry) {
                return entry.toJson();
            });

    auto ret_json =
        std::accumulate(
            std::make_move_iterator(std::begin(json_entrys)),
            std::make_move_iterator(std::end(json_entrys)),
            Json::Value{Json::ValueType::arrayValue},
            [](auto init, auto entry) {
                init.append(std::move(entry));
                return init;
            });
    return ret_json;
}

auto ReadWriteWalletServer::getbalanceof(bool isstring,
                                         const std::string& owner,
                                         const std::string& token)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, token);

    auto balance =
        lookup.getUtilityTokenCreditOf(owner,
                                       key_vec);

    return fmt::format("{}", balance);
}

auto ReadWriteWalletServer::getsupplyofutilitytoken(bool isstring,
                                                    const std::string& token)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& lookup = getLookup();

    auto key_vec = extractEntryKey(isstring, token);

    auto supply = lookup.getSupplyOfToken(key_vec);

    return fmt::format("{}", supply);
}

auto ReadWriteWalletServer::createnewutilitytoken(const std::string& address,
                                                  int burnvalue,
                                                  bool is_string,
                                                  const std::string& id,
                                                  const std::string& supply_str)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadWriteWallet();

    auto supply = std::stoull(supply_str);

    auto id_vec = extractEntryKey(is_string, id);

    if(address.empty()) {
        auto res = wallet.createNewUtilityToken(std::move(id_vec),
                                                supply,
                                                burnvalue);


        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();
    }
    auto address_copy = address;
    auto res = wallet.createNewUtilityToken(std::move(id_vec),
                                            supply,
                                            std::move(address_copy),
                                            burnvalue);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::sendutilitytokens(const std::string& amount_str,
                                              int burnvalue,
                                              bool is_string,
                                              const std::string& token,
                                              const std::string& newowner)
    -> Json::Value
{
    auto& wallet = getReadWriteWallet();

    auto amount = std::stoull(amount_str);

    auto id_vec = extractEntryKey(is_string, token);

    auto res = wallet.transferUtilityTokens(id_vec,
                                            newowner,
                                            amount,
                                            burnvalue);
    if(!res) {
        throw JsonRpcException(res.getError().what());
    }

    auto txids = res.getValue();

    return std::accumulate(
        std::make_move_iterator(std::begin(txids)),
        std::make_move_iterator(std::end(txids)),
        Json::Value{Json::ValueType::arrayValue},
        [](auto init, auto entry) {
            init.append(std::move(entry));
            return init;
        });
}

auto ReadWriteWalletServer::burnutilitytokens(const std::string& amount_str,
                                              int burnvalue,
                                              bool is_string,
                                              const std::string& token)
    -> Json::Value
{
    auto& wallet = getReadWriteWallet();

    auto amount = std::stoull(amount_str);
    auto id_vec = extractEntryKey(is_string, token);

    auto res = wallet.deleteUtilityTokens(id_vec,
                                          amount,
                                          burnvalue);
    if(!res) {
        throw JsonRpcException(res.getError().what());
    }

    auto txids = res.getValue();

    return std::accumulate(
        std::make_move_iterator(std::begin(txids)),
        std::make_move_iterator(std::end(txids)),
        Json::Value{Json::ValueType::arrayValue},
        [](auto init, auto entry) {
            init.append(std::move(entry));
            return init;
        });
}

auto ReadWriteWalletServer::hasShutdownRequest() const
    -> bool
{
    return should_shutdown_.load();
}


auto ReadWriteWalletServer::getLookup()
    -> lookup::LookupManager&
{
    return std::visit(
        utilxx::overload{
            [](LookupManager& lookup)
                -> LookupManager& {
                return lookup;
            },
            [](auto& wallet)
                -> LookupManager& {
                return wallet.getLookup();
            }},
        logic_);
}

auto ReadWriteWalletServer::getReadOnlyWallet()
    -> wallet::ReadOnlyWallet&
{
    if(std::holds_alternative<LookupManager>(logic_)) {
        auto error = fmt::format("rpc server unable to perform this operation in mode {}", getMode());
        throw JsonRpcException(std::move(error));
    }

    if(std::holds_alternative<ReadOnlyWallet>(logic_)) {
        return std::get<ReadOnlyWallet>(logic_);
    }

    //only other possibility is a readwrite wallet
    return std::get<ReadWriteWallet>(logic_);
}

auto ReadWriteWalletServer::getReadWriteWallet()
    -> wallet::ReadWriteWallet&
{
    if(!std::holds_alternative<ReadWriteWallet>(logic_)) {
        auto error =
            fmt::format("rpc server unable to perform this operation in mode {}",
                        getMode());
        throw JsonRpcException(std::move(error));
    }

    return std::get<ReadWriteWallet>(logic_);
}

auto ReadWriteWalletServer::getMode() const
    -> std::string
{
    return std::visit(
        utilxx::overload{
            [](const LookupManager& /*unused*/) {
                return "lookuponly";
            },
            [](const ReadOnlyWallet& /*unused*/) {
                return "readonly";
            },
            [](const ReadWriteWallet& /*unused*/) {
                return "readwrite";
            }},
        logic_);
}

auto ReadWriteWalletServer::extractEntryKey(bool isstring,
                                            const std::string& key_str)
    -> core::EntryKey
{
    if(isstring) {
        return core::stringToASCIIByteVec(key_str);
    }

    auto vec_opt = core::stringToByteVec(key_str);
    if(!vec_opt) {
        throw JsonRpcException{"could not convert given bytestring into vector of byte"};
    }

    return vec_opt.getValue();
}

auto ReadWriteWalletServer::startUpdaterThread()
    -> void
{
    updater_ =
        std::thread{[this]() {
            auto& lookup = getLookup();

            auto blocktime = getBlockTimeInSeconds(lookup.getCoin());
            std::chrono::seconds sleeptime{blocktime / 2};

            while(!should_shutdown_.load()) {

                indexing_.store(true);
                lookup.updateLookup();
                indexing_.store(false);

                std::this_thread::sleep_for(sleeptime);
            }
        }};
}


auto forge::rpc::waitForShutdown(const ReadWriteWalletServer& server)
    -> void
{
    using namespace std::literals::chrono_literals;

    while(!server.hasShutdownRequest()) {
        std::this_thread::sleep_for(1s);
    }
}
