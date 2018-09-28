#pragma once

#include <array>
#include <core/Entry.hpp>
#include <cstddef>
#include <vector>
#include <core/EntryCreationOp.hpp>
#include <core/EntryDeletionOp.hpp>
#include <core/EntryRenewalOp.hpp>
#include <core/EntryUpdateOp.hpp>
#include <core/OwnershipTransferOp.hpp>


namespace buddy::core {

constexpr static std::array<std::byte, 3> BUDDY_IDENTIFIER_MASK{(std::byte)0xC6,
                                                                (std::byte)0xDC,
                                                                (std::byte)0x75};


using Operation = std::variant<EntryCreationOp,
                               EntryRenewalOp,
                               OwnershipTransferOp,
                               EntryUpdateOp,
                               EntryDeletionOp>;

auto startsWithBUDDYIdentifier(const std::vector<std::byte>& data)
    -> bool;

} // namespace buddy::core
