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

auto createOwnershipTransferOpMetadata(Entry&& entry)
    -> std::vector<std::byte>;

auto createRenewalOpMetadata(RenewableEntry&& entry)
    -> std::vector<std::byte>;

auto createDeletionOpMetadata(Entry&& entry)
    -> std::vector<std::byte>;

} // namespace forge::core
