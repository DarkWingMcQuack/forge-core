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
#include <daemon/DaemonBase.hpp>
#include <memory>
#include <vector>


namespace buddy::core {

// clang-format off
constexpr auto ENTRY_CREATION_FLAG     = static_cast<std::byte>(0b00000001);
constexpr auto ENTRY_RENEWAL_FLAG      = static_cast<std::byte>(0b00000010);
constexpr auto OWNERSHIP_TRANSFER_FLAG = static_cast<std::byte>(0b00000100);
constexpr auto ENTRY_UPDATE_FLAG       = static_cast<std::byte>(0b00001000);
constexpr auto ENTRY_DELETION_FLAG     = static_cast<std::byte>(0b00010000);
// clang-format on

using Operation = std::variant<EntryCreationOp,
                               EntryRenewalOp,
                               OwnershipTransferOp,
                               EntryUpdateOp,
                               EntryDeletionOp>;

auto parseTransactionToEntry(core::Transaction&& tx,
                             std::size_t block,
                             const std::unique_ptr<daemon::DaemonBase>& daemon)
    -> util::Result<util::Opt<Operation>, daemon::DaemonError>;

auto parseMetadata(const std::vector<std::byte>& metadata,
                   std::size_t block,
                   std::string&& owner,
                   std::size_t value,
                   util::Opt<std::string>&& new_owner = std::nullopt)
    -> util::Opt<Operation>;

} // namespace buddy::core
