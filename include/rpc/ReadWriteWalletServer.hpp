#pragma once

#include <atomic>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/abstractreadwritewalletstubsever.h>
#include <thread>
#include <wallet/ReadWriteWallet.hpp>

namespace forge::rpc {

class ReadWriteWalletServer : public AbstractReadWriteWalletStubSever
{
public:
    ReadWriteWalletServer(jsonrpc::AbstractServerConnector& connector,
                          jsonrpc::serverVersion_t type,
                          wallet::ReadWriteWallet&& wallet);

    virtual auto updatelookup()
        -> bool override;

    virtual auto rebuildlookup()
        -> void override;

    virtual auto shutdown()
        -> void override;

    virtual auto lookupumvalue(bool isstring, const std::string& key)
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

    virtual auto paytoentryowner(int amount,
                                 bool is_string,
                                 const std::string& key)
        -> std::string override;

    auto hasShutdownRequest() const
        -> bool;

private:
    wallet::ReadWriteWallet wallet_;
    lookup::LookupManager& lookup_;
    std::atomic_bool should_shutdown_{false};
    std::atomic_bool indexing_{false};
    std::thread updater_;
};

auto waitForShutdown(const ReadWriteWalletServer& server)
    -> void;

} // namespace forge::rpc
