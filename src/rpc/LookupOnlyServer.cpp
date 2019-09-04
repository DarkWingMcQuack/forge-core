#include "core/Transaction.hpp"
#include "entrys/Entry.hpp"
#include <chrono>
#include <fmt/core.h>
#include <fmt/format.h>
#include <json/value.h>
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <numeric>
#include <rpc/LookupOnlyServer.hpp>
#include <thread>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Overload.hpp>

using forge::rpc::LookupOnlyServer;
using forge::core::getBlockTimeInSeconds;
using forge::core::EntryKey;
using forge::core::UMEntryValue;
using forge::core::IPv4Value;
using forge::core::IPv6Value;
using forge::core::ByteArray;
using jsonrpc::JsonRpcException;


LookupOnlyServer::LookupOnlyServer(jsonrpc::AbstractServerConnector& connector,
                                   jsonrpc::serverVersion_t type,
                                   std::unique_ptr<daemon::ReadOnlyDaemonBase>&& daemon)
    : AbstractLookupOnlyStubSever(connector, type),
      lookup_(std::move(daemon))
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


auto LookupOnlyServer::shutdown()
    -> void
{
    should_shutdown_.store(true);
}

auto LookupOnlyServer::updatelookup()
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

auto LookupOnlyServer::rebuildlookup()
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

auto LookupOnlyServer::lookupumvalue(bool isstring, const std::string& key)
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

    auto res = lookup_.lookupUMValue(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return umentryValueToJson(res.getValue().get());
}

auto LookupOnlyServer::lookupuniquevalue(bool isstring, const std::string& key)
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

    auto res = lookup_.lookupUniqueValue(key_vec);

    if(!res) {
        auto error_msg = fmt::format("no entrys with key {} found",
                                     key);
        throw JsonRpcException{std::move(error_msg)};
    }

    return uniqueEntryValueToJson(res.getValue().get());
}

auto LookupOnlyServer::lookupowner(bool isstring, const std::string& key)
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

auto LookupOnlyServer::lookupactivationblock(bool isstring, const std::string& key)
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

auto LookupOnlyServer::checkvalidity()
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

auto LookupOnlyServer::getlastvalidblockheight()
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

auto LookupOnlyServer::lookupallentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = lookup_.getEntrysOfOwner(owner);

    auto json_entrys =
        utilxx::transform_into_vector(std::make_move_iterator(std::begin(entrys)),
                                      std::make_move_iterator(std::end(entrys)),
                                      [](auto entry) {
                                          return core::entryToJson(std::move(entry));
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

auto LookupOnlyServer::lookupuniqueentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = lookup_.getUniqueEntrysOfOwner(owner);

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

auto LookupOnlyServer::lookupuniquemodifiableentrysof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto entrys = lookup_.getUMEntrysOfOwner(owner);

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

auto LookupOnlyServer::getutilitytokensof(const std::string& owner)
    -> Json::Value
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    auto tokens = lookup_.getUtilityTokensOfOwner(owner);

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

auto LookupOnlyServer::getbalanceof(bool isstring,
                                    const std::string& owner,
                                    const std::string& token)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    EntryKey key_vec;

    if(isstring) {
        std::transform(std::cbegin(token),
                       std::cend(token),
                       std::back_inserter(key_vec),
                       [](auto c) {
                           return static_cast<std::byte>(c);
                       });
    } else {
        auto vec_opt = core::stringToByteVec(token);
        if(!vec_opt) {
            throw JsonRpcException{"could not convert given bytestring into vector of byte"};
        }

        key_vec = std::move(vec_opt.getValue());
    }

    auto balance =
        lookup_.getUtilityTokenCreditOf(owner,
                                        core::toHexString(key_vec));

    return fmt::format("{}", balance);
}

auto LookupOnlyServer::getsupplyofutilitytoken(bool isstring,
                                               const std::string& token)
    -> std::string
{
    if(indexing_.load()) {
        throw JsonRpcException{"Server is indexing"};
    }

    EntryKey key_vec;

    if(isstring) {
        std::transform(std::cbegin(token),
                       std::cend(token),
                       std::back_inserter(key_vec),
                       [](auto c) {
                           return static_cast<std::byte>(c);
                       });
    } else {
        auto vec_opt = core::stringToByteVec(token);
        if(!vec_opt) {
            throw JsonRpcException{"could not convert given bytestring into vector of byte"};
        }

        key_vec = std::move(vec_opt.getValue());
    }

    auto supply = lookup_.getSupplyOfToken(core::toHexString(key_vec));

    return fmt::format("{}", supply);
}


auto LookupOnlyServer::hasShutdownRequest() const
    -> bool
{
    return should_shutdown_.load();
}

auto forge::rpc::waitForShutdown(const LookupOnlyServer& server)
    -> void
{
    using namespace std::literals::chrono_literals;

    while(!server.hasShutdownRequest()) {
        std::this_thread::sleep_for(1s);
    }
}
