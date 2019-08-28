#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <cstdint>
#include <daemon/DaemonError.hpp>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

using forge::core::UtilityToken;
using forge::core::Transaction;
using forge::core::UtilityTokenOperation;
using forge::daemon::ReadOnlyDaemonBase;
using forge::daemon::DaemonError;

auto forge::core::getUtilitToken(const UtilityTokenOperation& op)
    -> const UtilityToken&;
auto forge::core::getUtilityToken(UtilityTokenOperation&& op)
    -> UtilityToken;

auto forge::core::getAmount(const UtilityTokenOperation& op)
    -> std::uint64_t;

//extracts owner of the creator UtilityTokenOp from a given operaton
auto forge::core::getCreator(const UtilityTokenOperation&)
    -> const std::string&;
auto forge::core::getCreator(UtilityTokenOperation &&)
    -> std::string;

//extracts the operation flag which specifys which operation it is
auto forge::core::extractOperationFlag(const UtilityTokenOperation&)
    -> std::byte;


//checks the metadata of a transaction and parses it into
//an UtilityTokenOp if it holds the needed information
//and the metadata has the needed formating
auto forge::core::parseTransactionToUtilityTokenOp(Transaction tx,
                                                   std::int64_t block,
                                                   const ReadOnlyDaemonBase* daemon)
    -> utilxx::Result<utilxx::Opt<UtilityTokenOperation>,
                      DaemonError>;

//parses given metadata and constructs a UtilityTokenOp from
//the given information if possible
auto forge::core::parseMetadataToUtilityTokenOp(const std::vector<std::byte>& metadata,
                                                std::int64_t block,
                                                std::string&& owner,
                                                std::int64_t value,
                                                utilxx::Opt<std::string>&& new_owner)
    -> utilxx::Opt<UtilityTokenOperation>;
