#pragma once

#include <core/Operation.hpp>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadWriteDaemonBase.hpp>
#include <utilxx/Result.hpp>

namespace buddy::daemon {

class ReadWriteDaemonBase : public ReadOnlyDaemonBase
{
public:
    using ReadOnlyDaemonBase::ReadOnlyDaemonBase;

    virtual auto writeTxToBlockchain(std::string&& txid_input,
                                     std::vector<std::byte>&& metadata,
                                     std::size_t op_return_value,
                                     std::vector<
                                         std::pair<std::string,
                                                   std::size_t>>&& outputs)
        -> utilxx::Result<void, DaemonError> = 0;
};

auto make_daemon(const std::string& host,
                 const std::string& user,
                 const std::string& password,
                 std::size_t port,
                 Coin coin)
    -> std::unique_ptr<ReadWriteDaemonBase>;

} // namespace buddy::daemon
