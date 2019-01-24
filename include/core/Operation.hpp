#pragma once

#include <array>
#include <core/Entry.hpp>
#include <core/EntryCreationOp.hpp>
#include <core/EntryDeletionOp.hpp>
#include <core/EntryRenewalOp.hpp>
#include <core/EntryUpdateOp.hpp>
#include <core/OwnershipTransferOp.hpp>
#include <core/Transaction.hpp>
#include <cstddef>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <memory>
#include <vector>


namespace forge::core {

// clang-format off
constexpr inline auto ENTRY_CREATION_FLAG     = static_cast<std::byte>(0b00000001);
constexpr inline auto ENTRY_RENEWAL_FLAG      = static_cast<std::byte>(0b00000010);
constexpr inline auto OWNERSHIP_TRANSFER_FLAG = static_cast<std::byte>(0b00000100);
constexpr inline auto ENTRY_UPDATE_FLAG       = static_cast<std::byte>(0b00001000);
constexpr inline auto ENTRY_DELETION_FLAG     = static_cast<std::byte>(0b00010000);

constexpr inline auto ENTRY_OPERATION_FLAG_INDEX = 3;
// clang-format on

using Operation = std::variant<EntryCreationOp,
                               EntryRenewalOp,
                               OwnershipTransferOp,
                               EntryUpdateOp,
                               EntryDeletionOp>;

auto getEntryKey(const Operation&)
    -> const EntryKey&;
auto getEntryKey(Operation &&)
    -> EntryKey;

auto getEntry(const Operation&)
    -> const Entry&;
auto getEntry(Operation &&)
    -> Entry;

auto getOwner(const Operation&)
    -> const std::string&;
auto getOwner(Operation &&)
    -> std::string;

auto getValue(const Operation&)
    -> const std::int64_t;

auto extractFlag(const Operation&)
    -> std::byte;

auto parseTransactionToEntry(core::Transaction&& tx,
                             std::int64_t block,
                             const std::unique_ptr<daemon::ReadOnlyDaemonBase>& daemon)
    -> utilxx::Result<utilxx::Opt<Operation>, daemon::DaemonError>;

auto parseMetadata(const std::vector<std::byte>& metadata,
                   std::int64_t block,
                   std::string&& owner,
                   std::int64_t value,
                   utilxx::Opt<std::string>&& new_owner = std::nullopt)
    -> utilxx::Opt<Operation>;

auto operationToMetadata(const Operation& op)
    -> std::vector<std::byte>;

} // namespace forge::core
