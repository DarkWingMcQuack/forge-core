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

using Operation = std::variant<EntryCreationOp,
                               EntryRenewalOp,
                               OwnershipTransferOp,
                               EntryUpdateOp,
                               EntryDeletionOp>;

} // namespace buddy::core
