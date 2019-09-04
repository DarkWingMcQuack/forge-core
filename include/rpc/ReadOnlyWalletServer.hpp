#pragma once

#include <atomic>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/abstractreadonlywalletstubsever.h>
#include <thread>
#include <wallet/ReadOnlyWallet.hpp>

namespace forge::rpc {

class ReadOnlyWalletServer : public AbstractReadOnlyWalletStubSever
{
public:
    ReadOnlyWalletServer(jsonrpc::AbstractServerConnector& connector,
                         jsonrpc::serverVersion_t type,
                         wallet::ReadOnlyWallet&& wallet);

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

    virtual auto lookupuniqueentrysof(const std::string& owner)
        -> Json::Value override;

    virtual auto lookupuniquemodifiableentrysof(const std::string& owner)
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

    auto hasShutdownRequest() const
        -> bool;

private:
    wallet::ReadOnlyWallet wallet_;
    lookup::LookupManager& lookup_;
    std::atomic_bool should_shutdown_{false};
    std::atomic_bool indexing_{false};
    std::thread updater_;
};


auto waitForShutdown(const ReadOnlyWalletServer& server)
    -> void;

} // namespace forge::rpc
