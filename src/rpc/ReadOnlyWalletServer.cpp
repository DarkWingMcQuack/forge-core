#include <chrono>
#include <fmt/core.h>
#include <fmt/format.h>
#include <g3log/g3log.hpp>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <numeric>
#include <rpc/ReadOnlyWalletServer.hpp>
#include <thread>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Overload.hpp>
#include <wallet/ReadOnlyWallet.hpp>

using forge::rpc::ReadOnlyWalletServer;
using forge::core::getBlockTimeInSeconds;
using forge::core::Entry;
using forge::core::EntryKey;
using forge::core::EntryValue;
using forge::core::IPv4Value;
using forge::core::IPv6Value;
using forge::core::ByteArray;
using forge::core::NoneValue;
using forge::wallet::ReadOnlyWallet;
using jsonrpc::PARAMS_BY_NAME;
using jsonrpc::Procedure;
using jsonrpc::JsonRpcException;

ReadOnlyWalletServer::ReadOnlyWalletServer(jsonrpc::AbstractServerConnector& connector,
                                           jsonrpc::serverVersion_t type,
                                           wallet::ReadOnlyWallet&& wallet)
    : AbstractReadOnlyWalletStubSever(connector, type),
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


auto ReadOnlyWalletServer::updatelookup()
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

auto ReadOnlyWalletServer::rebuildlookup()
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

auto ReadOnlyWalletServer::shutdown()
    -> void
{
    should_shutdown_.store(true);
}

auto ReadOnlyWalletServer::lookupvalue(bool isstring, const std::string& key)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    EntryKey key_vec;

    if(isstring) {
        std::transform(std::cbegin(key),
                       std::cend(key),
                       std::back_inserter(key_vec),
                       [](auto c) {
                           return static_cast<std::byte>(c);
                       });
    } else {
        auto vec_opt = core::stringToByteVec(key);
        if(!vec_opt) {
            throw JsonRpcException{"could not convert given bytestring into vector of byte"};
        }

        key_vec = std::move(vec_opt.getValue());
    }

    auto res = lookup_.lookupValue(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return entryValueToJson(res.getValue().get());
}

auto ReadOnlyWalletServer::lookupowner(bool isstring, const std::string& key)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    EntryKey key_vec;

    if(isstring) {
        std::transform(std::cbegin(key),
                       std::cend(key),
                       std::back_inserter(key_vec),
                       [](auto c) {
                           return static_cast<std::byte>(c);
                       });
    } else {
        auto vec_opt = core::stringToByteVec(key);
        if(!vec_opt) {
            throw JsonRpcException{"could not convert given bytestring into vector of byte"};
        }

        key_vec = std::move(vec_opt.getValue());
    }

    auto res = lookup_.lookupOwner(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadOnlyWalletServer::lookupactivationblock(bool isstring, const std::string& key)
    -> int
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    EntryKey key_vec;

    if(isstring) {
        std::transform(std::cbegin(key),
                       std::cend(key),
                       std::back_inserter(key_vec),
                       [](auto c) {
                           return static_cast<std::byte>(c);
                       });
    } else {
        auto vec_opt = core::stringToByteVec(key);
        if(!vec_opt) {
            throw JsonRpcException{"could not convert given bytestring into vector of byte"};
        }

        key_vec = std::move(vec_opt.getValue());
    }

    auto res = lookup_.lookupActivationBlock(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return res.getValue();
}

auto ReadOnlyWalletServer::checkvalidity()
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

auto ReadOnlyWalletServer::lookupallentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = lookup_.getEntrysOfOwner(owner);

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entryToJson(std::move(entry));
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

auto ReadOnlyWalletServer::addwatchonlyaddress(const std::string& address)
    -> void
{
    auto copy = address;
    wallet_.addWatchOnlyAddress(std::move(copy));
}

auto ReadOnlyWalletServer::deletewatchonlyaddress(const std::string& address)
    -> void
{
    wallet_.deleteWatchOnlyAddress(address);
}

auto ReadOnlyWalletServer::addnewownedaddress(const std::string& address)
    -> void
{
    auto copy = address;
    wallet_.addNewOwnedAddress(std::move(copy));
}

auto ReadOnlyWalletServer::getownedentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = wallet_.getOwnedEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entryToJson(std::move(entry));
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

auto ReadOnlyWalletServer::getwatchonlyentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = wallet_.getWatchOnlyEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entryToJson(std::move(entry));
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

auto ReadOnlyWalletServer::getallwatchedentrys()
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = wallet_.getAllWatchedEntrys();

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto&& entry) {
                                          return entryToJson(std::move(entry));
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

auto ReadOnlyWalletServer::getwatchedaddresses()
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
auto ReadOnlyWalletServer::getownedaddresses()
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

auto ReadOnlyWalletServer::ownesaddress(const std::string& address)
    -> bool
{
    return wallet_.ownesAddress(address);
}

auto ReadOnlyWalletServer::hasShutdownRequest() const
    -> bool
{
    return should_shutdown_.load();
}

auto forge::rpc::waitForShutdown(const ReadOnlyWalletServer& server)
    -> void
{
    using namespace std::literals::chrono_literals;

    while(!server.hasShutdownRequest()) {
        std::this_thread::sleep_for(1s);
    }
}
