#pragma once

#include <array>
#include <cstddef>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <json/value.h>
#include <utilxx/Opt.hpp>
#include <variant>
#include <vector>

namespace forge::core {

using Entry = std::variant<UniqueEntry,
                           UMEntry>;

auto parseEntry(const std::vector<std::byte>& data)
    -> utilxx::Opt<Entry>;

auto entryToRawData(const Entry& entry)
    -> std::vector<std::byte>;

auto entryToJson(Entry entry)
    -> Json::Value;

} // namespace forge::core
