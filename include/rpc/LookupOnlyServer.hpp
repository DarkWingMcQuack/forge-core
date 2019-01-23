#pragma once

#include <atomic>
#include <jsonrpccpp/server/connectors/httpserver.h>
#include <lookup/LookupManager.hpp>
#include <rpc/abstractlookuponlystubsever.h>
#include <thread>

namespace forge::rpc {

class LookupOnlyServer : public AbstractLookupOnlyStubSever
{
public:
    LookupOnlyServer(jsonrpc::AbstractServerConnector& connector,
                     jsonrpc::serverVersion_t type,
                     std::unique_ptr<daemon::ReadOnlyDaemonBase>&& daemon);


    virtual auto updatelookup()
        -> bool override;

    virtual auto rebuildlookup()
        -> void override;

    virtual auto shutdown()
        -> void override;

    virtual auto lookupvalue(bool isstring, const std::string& key)
        -> Json::Value override;

    virtual auto lookupowner(bool isstring, const std::string& key)
        -> std::string override;

    virtual auto lookupactivationblock(bool isstring, const std::string& key)
        -> int override;

    virtual auto checkvalidity()
        -> bool override;

    virtual auto lookupallentrysof(const std::string& owner)
        -> Json::Value override;

    auto hasShutdownRequest() const
        -> bool;

private:
    lookup::LookupManager lookup_;
    std::atomic_bool indexing_{false};
    std::atomic_bool should_shutdown_{false};
    std::thread updater_;
};

auto waitForShutdown(const LookupOnlyServer& server)
    -> void;

} // namespace forge::rpc
