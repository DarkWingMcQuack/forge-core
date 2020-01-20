#include <chrono>
#include <core/Transaction.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <g3log/g3log.hpp>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <mutex>
#include <numeric>
#include <rpc/JsonRpcServer.hpp>
#include <thread>
#include <utils/Algorithm.hpp>
#include <utils/Overload.hpp>
#include <variant>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>

using forge::rpc::JsonRpcServer;
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

JsonRpcServer::JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                             jsonrpc::serverVersion_t type,
                             wallet::ReadWriteWallet&& wallet)
    : AbstractJsonRpcStubSever(connector, type),
      logic_(std::move(wallet))
{
    startUpdaterThread();
}

JsonRpcServer::JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                             jsonrpc::serverVersion_t type,
                             wallet::ReadOnlyWallet&& wallet)
    : AbstractJsonRpcStubSever(connector, type),
      logic_(std::move(wallet))
{
    startUpdaterThread();
}

JsonRpcServer::JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                             jsonrpc::serverVersion_t type,
                             lookup::LookupManager&& lookup)
    : AbstractJsonRpcStubSever(connector, type),
      logic_(std::move(lookup))
{
    startUpdaterThread();
}


auto JsonRpcServer::updatelookup()
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

auto JsonRpcServer::rebuildlookup()
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

auto JsonRpcServer::shutdown()
    -> void
{
    should_shutdown_.store(true);
}

auto JsonRpcServer::lookupumvalue(bool isstring, const std::string& key)
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

auto JsonRpcServer::lookupuniquevalue(bool isstring, const std::string& key)
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

auto JsonRpcServer::lookupowner(bool isstring, const std::string& key)
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

auto JsonRpcServer::lookupactivationblock(bool isstring, const std::string& key)
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

auto JsonRpcServer::checkvalidity()
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

auto JsonRpcServer::getlastvalidblockheight()
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

auto JsonRpcServer::lookupallentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& lookup = getLookup();

    auto entrys = lookup.getUMEntrysOfOwner(owner);

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::addwatchonlyaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    auto copy = address;
    wallet.addWatchOnlyAddress(std::move(copy));
}

auto JsonRpcServer::deletewatchonlyaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    wallet.deleteWatchOnlyAddress(address);
}

auto JsonRpcServer::addnewownedaddress(const std::string& address)
    -> void
{
    auto& wallet = getReadOnlyWallet();
    auto copy = address;
    wallet.addNewOwnedAddress(std::move(copy));
}

auto JsonRpcServer::getownedumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getOwnedUMEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getwatchonlyumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getWatchOnlyUMEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getallwatchedumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getAllWatchedUMEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getowneduniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getOwnedUniqueEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getwatchonlyuniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getWatchOnlyUniqueEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getallwatcheduniqueentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadOnlyWallet();
    auto entrys = wallet.getAllWatchedUniqueEntrys();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getwatchedaddresses()
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
auto JsonRpcServer::getownedaddresses()
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

auto JsonRpcServer::ownesaddress(const std::string& address)
    -> bool
{
    const auto& wallet = getReadOnlyWallet();
    return wallet.ownesAddress(address);
}

auto JsonRpcServer::createnewumentry(const std::string& address,
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


auto JsonRpcServer::updateumentry(int burnvalue,
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


auto JsonRpcServer::createnewuniqueentry(const std::string& address,
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

auto JsonRpcServer::renewentry(int burnvalue,
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

auto JsonRpcServer::deleteentry(int burnvalue,
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

auto JsonRpcServer::transferownership(int burnvalue,
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

auto JsonRpcServer::paytoentryowner(int amount,
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

auto JsonRpcServer::getownedutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();

    auto entrys = wallet.getOwnedUtilityTokens();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getwatchonlyutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();

    auto entrys = wallet.getWatchOnlyUtilityTokens();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getallwatchedutilitytokens()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& wallet = getReadWriteWallet();
    auto entrys = wallet.getAllWatchedUtilityTokens();

    auto json_entrys =
        utils::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
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

auto JsonRpcServer::getutilitytokensof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    const auto& lookup = getLookup();
    auto tokens = lookup.getUtilityTokensOfOwner(owner);

    auto json_entrys =
        utils::transform_into_vector(
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

auto JsonRpcServer::getbalanceof(bool isstring,
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

auto JsonRpcServer::getsupplyofutilitytoken(bool isstring,
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

auto JsonRpcServer::createnewutilitytoken(const std::string& address,
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

auto JsonRpcServer::sendutilitytokens(const std::string& amount_str,
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

auto JsonRpcServer::burnutilitytokens(const std::string& amount_str,
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

auto JsonRpcServer::hasShutdownRequest() const
    -> bool
{
    return should_shutdown_.load();
}


auto JsonRpcServer::getLookup()
    -> lookup::LookupManager&
{
    return std::visit(
        utils::overload{
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

auto JsonRpcServer::getReadOnlyWallet()
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

auto JsonRpcServer::getReadWriteWallet()
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

auto JsonRpcServer::getMode() const
    -> std::string
{
    return std::visit(
        utils::overload{
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

auto JsonRpcServer::extractEntryKey(bool isstring,
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

auto JsonRpcServer::startUpdaterThread()
    -> void
{
    updater_ =
        std::thread{[this] {
            auto& lookup = getLookup();

            auto blocktime = getBlockTimeInSeconds(lookup.getCoin());
            std::chrono::seconds sleeptime{blocktime / 2};
            std::mutex mtx;

            while(!should_shutdown_.load()) {

                indexing_.store(true);
                lookup.updateLookup();
                indexing_.store(false);
                std::unique_lock lock{mtx};
                shutdown_requested_.wait_for(lock, sleeptime);
            }
        }};
}

JsonRpcServer::~JsonRpcServer()
{
    should_shutdown_ = true;
    shutdown_requested_.notify_all();
    updater_.join();
}


auto forge::rpc::waitForShutdown(const JsonRpcServer& server)
    -> void
{
    using namespace std::literals::chrono_literals;

    while(!server.hasShutdownRequest()) {
        std::this_thread::sleep_for(1s);
    }
}
