#pragma once

#include <atomic>
#include <entrys/token/UtilityToken.hpp>
#include <json/value.h>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/abstractjsonrpcstubserver.h>
#include <thread>
#include <variant>
#include <wallet/ReadOnlyWallet.hpp>
#include <wallet/ReadWriteWallet.hpp>

namespace forge::rpc {

class JsonRpcServer : public AbstractJsonRpcStubSever
{
public:
    JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                  jsonrpc::serverVersion_t type,
                  wallet::ReadWriteWallet&& wallet);

    JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                  jsonrpc::serverVersion_t type,
                  wallet::ReadOnlyWallet&& wallet);

    JsonRpcServer(jsonrpc::AbstractServerConnector& connector,
                  jsonrpc::serverVersion_t type,
                  lookup::LookupManager&& lookup);

    virtual ~JsonRpcServer();

    virtual auto updatelookup()
        -> bool override;

    virtual auto rebuildlookup()
        -> void override;

    virtual auto shutdown()
        -> void override;

    virtual auto lookupumvalue(bool isstring, const std::string& key)
        -> Json::Value override;

    virtual auto lookupuniquevalue(bool isstring, const std::string& key)
        -> Json::Value override;

    virtual auto lookupowner(bool isstring, const std::string& key)
        -> std::string override;

    virtual auto lookupactivationblock(bool isstring, const std::string& key)
        -> int override;

    virtual auto checkvalidity()
        -> bool override;

    virtual auto getlastvalidblockheight()
        -> int override;

    virtual auto lookupallentrysof(const std::string& owner)
        -> Json::Value override;

    virtual auto getutilitytokensof(const std::string& owner)
        -> Json::Value override;

    virtual auto getbalanceof(bool isstring,
                              const std::string& owner,
                              const std::string& token)
        -> std::string override;

    virtual auto getsupplyofutilitytoken(bool isstring,
                                         const std::string& token)
        -> std::string override;

    virtual auto addwatchonlyaddress(const std::string& address)
        -> void override;
    virtual auto deletewatchonlyaddress(const std::string& address)
        -> void override;
    virtual auto addnewownedaddress(const std::string& address)
        -> void override;
    virtual auto getownedumentrys()
        -> Json::Value override;
    virtual auto getwatchonlyumentrys()
        -> Json::Value override;
    virtual auto getallwatchedumentrys()
        -> Json::Value override;
    virtual auto getowneduniqueentrys()
        -> Json::Value override;
    virtual auto getwatchonlyuniqueentrys()
        -> Json::Value override;
    virtual auto getallwatcheduniqueentrys()
        -> Json::Value override;
    virtual auto getwatchedaddresses()
        -> Json::Value override;
    virtual auto getownedaddresses()
        -> Json::Value override;
    virtual auto ownesaddress(const std::string& address)
        -> bool override;
    virtual auto getownedutilitytokens()
        -> Json::Value override;
    virtual auto getwatchonlyutilitytokens()
        -> Json::Value override;
    virtual auto getallwatchedutilitytokens()
        -> Json::Value override;

    virtual auto createnewumentry(const std::string& address,
                                  int burnvalue,
                                  bool is_string,
                                  const std::string& key,
                                  const Json::Value& value)
        -> std::string override;

    virtual auto updateumentry(int burnvalue,
                               bool is_string,
                               const std::string& key,
                               const Json::Value& value)
        -> std::string override;

    virtual auto createnewuniqueentry(const std::string& address,
                                      int burnvalue,
                                      bool is_string,
                                      const std::string& key,
                                      const Json::Value& value)
        -> std::string override;

    virtual auto createnewutilitytoken(const std::string& address,
                                       int burnvalue,
                                       bool is_string,
                                       const std::string& id,
                                       const std::string& supply_str)
        -> std::string override;

    virtual auto renewentry(int burnvalue,
                            bool is_string,
                            const std::string& key)
        -> std::string override;


    virtual auto deleteentry(int burnvalue,
                             bool is_string,
                             const std::string& key)
        -> std::string override;

    virtual auto transferownership(int burnvalue,
                                   bool is_string,
                                   const std::string& key,
                                   const std::string& newowner)
        -> std::string override;

    virtual auto sendutilitytokens(const std::string& amount_str,
                                   int burnvalue,
                                   bool is_string,
                                   const std::string& token,
                                   const std::string& newowner)
        -> Json::Value override;

    virtual auto burnutilitytokens(const std::string& amount_str,
                                   int burnvalue,
                                   bool is_string,
                                   const std::string& token)
        -> Json::Value override;

    virtual auto paytoentryowner(int amount,
                                 bool is_string,
                                 const std::string& key)
        -> std::string override;

    auto hasShutdownRequest() const
        -> bool;

private:
    auto getLookup()
        -> lookup::LookupManager&;

    auto getReadOnlyWallet()
        -> wallet::ReadOnlyWallet&;

    auto getReadWriteWallet()
        -> wallet::ReadWriteWallet&;

    auto getMode() const
        -> std::string;

    auto startUpdaterThread()
        -> void;

    auto extractEntryKey(bool isstring,
                         const std::string& key_str)
        -> core::EntryKey;

private:
    // wallet::ReadWriteWallet wallet_;
    std::variant<wallet::ReadWriteWallet,
                 wallet::ReadOnlyWallet,
                 lookup::LookupManager>
        logic_;
    // lookup::LookupManager& lookup_;
    std::atomic_bool should_shutdown_{false};
    std::atomic_bool indexing_{false};
    std::thread updater_;
    std::condition_variable shutdown_requested_;
};

auto waitForShutdown(const JsonRpcServer& server)
    -> void;

} // namespace forge::rpc
