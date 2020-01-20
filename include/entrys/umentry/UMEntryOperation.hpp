#pragma once

#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <client/ReadOnlyClientBase.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryCreationOp.hpp>
#include <entrys/umentry/UMEntryDeletionOp.hpp>
#include <entrys/umentry/UMEntryOwnershipTransferOp.hpp>
#include <entrys/umentry/UMEntryRenewalOp.hpp>
#include <entrys/umentry/UMEntryUpdateOp.hpp>
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

//extracts the EntryKey from a given operaton
auto getEntryKey(const UMEntryOperation&)
    -> const EntryKey&;
auto getEntryKey(UMEntryOperation &&)
    -> EntryKey;

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
    -> std::int64_t;

//extracts the operation flag which specifys which operation it is
auto extractOperationFlag(const UMEntryOperation&)
    -> std::byte;

//checks the metadata of a transaction and parses it into
//an UMEntryOperation if it holds the needed information
//and the metadata has the needed formating
auto parseTransactionToUMEntry(core::Transaction tx,
                               std::int64_t block,
                               const client::ReadOnlyClientBase* client)
    -> utils::Result<utils::Opt<UMEntryOperation>, client::ClientError>;

//parses given metadata and constructs a UNEntryOperation from
//the given information if possible
auto parseMetadataToUMEntryOp(const std::vector<std::byte>& metadata,
                              std::int64_t block,
                              std::string&& owner,
                              std::int64_t value,
                              utils::Opt<std::string>&& new_owner = std::nullopt)
    -> utils::Opt<UMEntryOperation>;

//given a UMEntryOperation, this function builds a matching
//metadata which can be used to write it on the blockchain
auto toMetadata(const UMEntryOperation& op)
    -> std::vector<std::byte>;

} // namespace forge::core
