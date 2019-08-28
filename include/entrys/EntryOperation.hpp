#pragma once

#include <array>
#include <core/Transaction.hpp>
#include <cstddef>
#include <daemon/ReadOnlyDaemonBase.hpp>
#include <entrys/Entry.hpp>
#include <entrys/uentry/UniqueEntryOperation.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <memory>
#include <variant>
#include <vector>


namespace forge::core {

using EntryOperation = std::variant<UMEntryOperation,
                                    UniqueEntryOperation>;


auto createOwnershipTransferOpMetadata(const Entry& entryA)
    -> std::vector<std::byte>;

auto createRenewalOpMetadata(const Entry& entryA)
    -> std::vector<std::byte>;

auto createDeletionOpMetadata(const Entry& entryA)
    -> std::vector<std::byte>;

//given a EntryOperation, this function builds a matching
//metadata which can be used to write it on the blockchain
auto toMetadata(const EntryOperation& op)
    -> std::vector<std::byte>;

} // namespace forge::core
