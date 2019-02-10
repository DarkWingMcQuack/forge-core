#pragma once

#include <array>
#include <core/Transaction.hpp>
#include <core/umentry/UMEntry.hpp>
#include <core/umentry/UMEntryCreationOp.hpp>
#include <core/umentry/UMEntryDeletionOp.hpp>
#include <core/umentry/UMEntryOwnershipTransferOp.hpp>
#include <core/umentry/UMEntryRenewalOp.hpp>
#include <core/umentry/UMEntryUpdateOp.hpp>
#include <cstddef>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <memory>
#include <vector>


namespace forge::core {

// clang-format off
constexpr inline auto UMENTRY_CREATION_FLAG     = static_cast<std::byte>(0b00000001);
constexpr inline auto UMENTRY_RENEWAL_FLAG      = static_cast<std::byte>(0b00000010);
constexpr inline auto UMENTRY_OWNERSHIP_TRANSFER_FLAG = static_cast<std::byte>(0b00000100);
constexpr inline auto UMENTRY_UPDATE_FLAG       = static_cast<std::byte>(0b00001000);
constexpr inline auto UMENTRY_DELETION_FLAG     = static_cast<std::byte>(0b00010000);
// clang-format on

using UMEntryOperation = std::variant<UMEntryCreationOp,
                                      UMEntryRenewalOp,
                                      UMEntryOwnershipTransferOp,
                                      UMEntryUpdateOp,
                                      UMEntryDeletionOp>;

//extracts the UMEntryKey from a given operaton
auto getUMEntryKey(const UMEntryOperation&)
    -> const UMEntryKey&;
auto getUMEntryKey(UMEntryOperation &&)
    -> UMEntryKey;

//extracts the UMEntry from a given operaton
auto getUMEntry(const UMEntryOperation&)
    -> const UMEntry&;
auto getUMEntry(UMEntryOperation &&)
    -> UMEntry;

//extracts owner of the UMEntry from a given operaton
auto getOwner(const UMEntryOperation&)
    -> const std::string&;
auto getOwner(UMEntryOperation &&)
    -> std::string;

//extracts burn value with which the given operation was
//burned onto the chain
auto getValue(const UMEntryOperation&)
    -> const std::int64_t;

//extracts the operation flag which specifys which operation it is
auto extractOperationFlag(const UMEntryOperation&)
    -> std::byte;

//checks the metadata of a transaction and parses it into
//an UMEntryOperation if it holds the needed information
//and the metadata has the needed formating
auto parseTransactionToUMEntry(core::Transaction&& tx,
                               std::int64_t block,
                               const daemon::ReadOnlyDaemonBase* daemon)
    -> utilxx::Result<utilxx::Opt<UMEntryOperation>, daemon::DaemonError>;

//parses given metadata and constructs a UNEntryOperation from
//the given information if possible
auto parseMetadata(const std::vector<std::byte>& metadata,
                   std::int64_t block,
                   std::string&& owner,
                   std::int64_t value,
                   utilxx::Opt<std::string>&& new_owner = std::nullopt)
    -> utilxx::Opt<UMEntryOperation>;

//given a UMEntryOperation, this function builds a matching
//metadata which can be used to write it on the blockchain
auto toMetadata(const UMEntryOperation& op)
    -> std::vector<std::byte>;

} // namespace forge::core
