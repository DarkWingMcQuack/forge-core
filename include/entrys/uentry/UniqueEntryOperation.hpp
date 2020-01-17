#pragma once

#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <client/ReadOnlyClientBase.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/uentry/UniqueEntryCreationOp.hpp>
#include <entrys/uentry/UniqueEntryDeletionOp.hpp>
#include <entrys/uentry/UniqueEntryOwnershipTransferOp.hpp>
#include <entrys/uentry/UniqueEntryRenewalOp.hpp>
#include <memory>
#include <vector>


namespace forge::core {

// clang-format off
constexpr inline auto UNIQUE_ENTRY_CREATION_FLAG           = static_cast<std::byte>(0b00000001);
constexpr inline auto UNIQUE_ENTRY_RENEWAL_FLAG            = static_cast<std::byte>(0b00000010);
constexpr inline auto UNIQUE_ENTRY_OWNERSHIP_TRANSFER_FLAG = static_cast<std::byte>(0b00000100);
constexpr inline auto UNIQUE_ENTRY_DELETION_FLAG           = static_cast<std::byte>(0b00001000);
// clang-format on

using UniqueEntryOperation = std::variant<UniqueEntryCreationOp,
                                          UniqueEntryRenewalOp,
                                          UniqueEntryOwnershipTransferOp,
                                          UniqueEntryDeletionOp>;

//extracts the EntryKey from a given operaton
auto getEntryKey(const UniqueEntryOperation&)
    -> const EntryKey&;
auto getEntryKey(UniqueEntryOperation &&)
    -> EntryKey;

//extracts the UniqueEntry from a given operaton
auto getUniqueEntry(const UniqueEntryOperation&)
    -> const UniqueEntry&;
auto getUniqueEntry(UniqueEntryOperation &&)
    -> UniqueEntry;

//extracts owner of the UniqueEntry from a given operaton
auto getOwner(const UniqueEntryOperation&)
    -> const std::string&;
auto getOwner(UniqueEntryOperation &&)
    -> std::string;

//extracts burn value with which the given operation was
//burned onto the chain
auto getValue(const UniqueEntryOperation&)
    -> std::int64_t;

//extracts the operation flag which specifys which operation it is
auto extractOperationFlag(const UniqueEntryOperation&)
    -> std::byte;

//checks the metadata of a transaction and parses it into
//an UniqueEntryOperation if it holds the needed information
//and the metadata has the needed formating
auto parseTransactionToUniqueEntry(core::Transaction tx,
                                   std::int64_t block,
                                   const client::ReadOnlyClientBase* client)
    -> utilxx::Result<utilxx::Opt<UniqueEntryOperation>, client::ClientError>;

//parses given metadata and constructs a UniqueEntryOperation from
//the given information if possible
auto parseMetadataToUniqueEntryOp(const std::vector<std::byte>& metadata,
                                  std::int64_t block,
                                  std::string&& owner,
                                  std::int64_t value,
                                  utilxx::Opt<std::string>&& new_owner = std::nullopt)
    -> utilxx::Opt<UniqueEntryOperation>;

//given a UniqueEntryOperation, this function builds a matching
//metadata which can be used to write it on the blockchain
auto toMetadata(const UniqueEntryOperation& op)
    -> std::vector<std::byte>;

} // namespace forge::core
