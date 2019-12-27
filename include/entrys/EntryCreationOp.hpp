#pragma once

#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/uentry/UniqueEntryCreationOp.hpp>
#include <entrys/umentry/UMEntryCreationOp.hpp>
#include <variant>

namespace forge::core {

using EntryCreationOp = std::variant<UniqueEntryCreationOp,
                                     UMEntryCreationOp,
                                     UtilityTokenCreationOp>;

auto getBurnValue(const EntryCreationOp& op)
    -> bool;
} // namespace forge::core
