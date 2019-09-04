#pragma once

#include <array>
#include <cstddef>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

namespace forge::core {

using Entry = std::variant<UniqueEntry,
                           UMEntry,
                           UtilityToken>;

auto parseEntry(const std::vector<std::byte>& data)
    -> utilxx::Opt<Entry>;

auto entryToRawData(const Entry& entry)
    -> std::vector<std::byte>;

auto entryToJson(const Entry& entry)
    -> Json::Value;

} // namespace forge::core
