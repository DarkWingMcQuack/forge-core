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
#include <wallet/ReadWriteWallet.hpp>

using forge::rpc::ReadWriteWalletServer;
using forge::core::getBlockTimeInSeconds;
using forge::core::UMEntry;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::IPv4Value;
using forge::core::IPv6Value;
using forge::core::ByteArray;
using forge::core::NoneValue;
using forge::wallet::ReadWriteWallet;
using jsonrpc::PARAMS_BY_NAME;
using jsonrpc::Procedure;
using jsonrpc::JsonRpcException;

ReadWriteWalletServer::ReadWriteWalletServer(jsonrpc::AbstractServerConnector& connector,
                                             jsonrpc::serverVersion_t type,
                                             wallet::ReadWriteWallet&& wallet)
    : AbstractReadWriteWalletStubSever(connector, type),
      wallet_(std::move(wallet)),
      lookup_(wallet_.getLookup())
{
    //start an updater thread which updates the lookup in the background
    updater_ =
        std::thread{[this]() {
            auto blocktime = getBlockTimeInSeconds(lookup_.getCoin());
            std::chrono::seconds sleeptime{blocktime / 2};

            while(!should_shutdown_.load()) {

                indexing_.store(true);
                lookup_.updateLookup();
                indexing_.store(false);

                std::this_thread::sleep_for(sleeptime);
            }
        }};
}


auto ReadWriteWalletServer::updatelookup()
    -> bool
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is already indexing"};
    }

    indexing_.store(true);

    auto res = lookup_.updateLookup();

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

    indexing_.store(true);
    auto res = lookup_.rebuildLookup();
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

    auto key_vec_opt =
        [&] {
            if(isstring) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();

    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = lookup_.lookupUMValue(key_vec);

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

    auto key_vec_opt =
        [&] {
            if(isstring) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = lookup_.lookupOwner(key_vec);

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

    auto key_vec_opt =
        [&] {
            if(isstring) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();

    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = lookup_.lookupActivationBlock(key_vec);

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

    auto res = lookup_.lookupIsValid();

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

    auto res = lookup_.getLastValidBlockHeight();

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

    auto entrys = lookup_.getUMEntrysOfOwner(owner);

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::addwatchonlyaddress(const std::string& address)
    -> void
{
    auto copy = address;
    wallet_.addWatchOnlyAddress(std::move(copy));
}

auto ReadWriteWalletServer::deletewatchonlyaddress(const std::string& address)
    -> void
{
    wallet_.deleteWatchOnlyAddress(address);
}

auto ReadWriteWalletServer::addnewownedaddress(const std::string& address)
    -> void
{
    auto copy = address;
    wallet_.addNewOwnedAddress(std::move(copy));
}

auto ReadWriteWalletServer::getownedumentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = wallet_.getOwnedUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
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

    auto entrys = wallet_.getWatchOnlyUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
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

    auto entrys = wallet_.getAllWatchedUMEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
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

    auto entrys = wallet_.getOwnedUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
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

    auto entrys = wallet_.getWatchOnlyUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
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

    auto entrys = wallet_.getAllWatchedUniqueEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entry.toJson();
                                      });

    auto ret_json =
        std::accumulate(std::make_move_iterator(std::begin(json_entrys)),
                        std::make_move_iterator(std::end(json_entrys)),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, auto&& entry) {
                            init.append(std::move(entry));
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::getwatchedaddresses()
    -> Json::Value
{
    const auto& addresses = wallet_.getWatchedAddresses();
    auto ret_json =
        std::accumulate(std::begin(addresses),
                        std::end(addresses),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, const auto& entry) {
                            init.append(entry);
                            return init;
                        });

    return ret_json;
}
auto ReadWriteWalletServer::getownedaddresses()
    -> Json::Value
{
    const auto& addresses = wallet_.getOwnedAddresses();
    auto ret_json =
        std::accumulate(std::begin(addresses),
                        std::end(addresses),
                        Json::Value{Json::ValueType::arrayValue},
                        [](auto&& init, const auto& entry) {
                            init.append(entry);
                            return init;
                        });

    return ret_json;
}

auto ReadWriteWalletServer::ownesaddress(const std::string& address)
    -> bool
{
    return wallet_.ownesAddress(address);
}

auto ReadWriteWalletServer::createnewumentry(const std::string& address,
                                             int burnvalue,
                                             bool is_string,
                                             const std::string& key,
                                             const Json::Value& value)
    -> std::string
{
    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUMEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();

    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto entry_value = std::move(entry_value_opt.getValue());
    auto key_vec = std::move(key_vec_opt.getValue());

    if(address.empty()) {
        auto res = wallet_.createNewUMEntry(std::move(key_vec),
                                            std::move(entry_value),
                                            burnvalue);


        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();

    } else {
        auto address_copy = address;
        auto res = wallet_.createNewUMEntry(std::move(key_vec),
                                            std::move(entry_value),
                                            std::move(address_copy),
                                            burnvalue);

        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();
    }
}


auto ReadWriteWalletServer::updateumentry(int burnvalue,
                                          bool is_string,
                                          const std::string& key,
                                          const Json::Value& value)
    -> std::string
{
    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUMEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto entry_value = std::move(entry_value_opt.getValue());
    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = wallet_.updateUMEntry(std::move(key_vec),
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
    auto value_copy = value;
    auto entry_value_opt = forge::core::jsonToUniqueEntryValue(std::move(value_copy));
    if(!entry_value_opt.hasValue()) {
        throw JsonRpcException{"unable to decode value"};
    }

    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();

    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto entry_value = std::move(entry_value_opt.getValue());
    auto key_vec = std::move(key_vec_opt.getValue());

    if(address.empty()) {
        auto res = wallet_.createNewUniqueEntry(std::move(key_vec),
                                                std::move(entry_value),
                                                burnvalue);


        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();

    } else {
        auto address_copy = address;
        auto res = wallet_.createNewUniqueEntry(std::move(key_vec),
                                                std::move(entry_value),
                                                std::move(address_copy),
                                                burnvalue);

        if(!res) {
            auto error_msg = res.getError().what();
            throw JsonRpcException{std::move(error_msg)};
        }

        return res.getValue();
    }
}

auto ReadWriteWalletServer::renewentry(int burnvalue,
                                       bool is_string,
                                       const std::string& key)
    -> std::string
{
    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = wallet_.renewEntry(std::move(key_vec),
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
    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = wallet_.deleteEntry(std::move(key_vec),
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
    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto newowner_copy = newowner;

    auto res = wallet_.transferOwnership(std::move(key_vec),
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
    auto key_vec_opt =
        [&] {
            if(is_string) {
                auto byte_vec = forge::core::stringToASCIIByteVec(key);
                return utilxx::Opt{byte_vec};
            } else {
                return forge::core::stringToByteVec(key);
            }
        }();
    if(!key_vec_opt.hasValue()) {
        throw JsonRpcException{"unable to decode key"};
    }

    auto key_vec = std::move(key_vec_opt.getValue());

    auto res = wallet_.payToEntryOwner(std::move(key_vec),
                                       amount);

    if(!res) {
        auto error_msg = res.getError().what();
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadWriteWalletServer::hasShutdownRequest() const
    -> bool
{
    return should_shutdown_.load();
}


auto forge::rpc::waitForShutdown(const ReadWriteWalletServer& server)
    -> void
{
    using namespace std::literals::chrono_literals;

    while(!server.hasShutdownRequest()) {
        std::this_thread::sleep_for(1s);
    }
}
