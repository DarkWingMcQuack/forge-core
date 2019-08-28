#include <entrys/Entry.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Overload.hpp>
#include <variant>
#include <vector>

using forge::core::Entry;

auto forge::core::parseEntry(const std::vector<std::byte>& data)
    -> utilxx::Opt<Entry>
{
    auto um_entry_opt = parseUMEntry(data);
    if(um_entry_opt) {
        return um_entry_opt
            .map([](auto&& entry) {
                return Entry{entry};
            });
    }

    return parseUniqueEntry(data)
        .map([](auto&& entry) {
            return Entry{std::move(entry)};
        });
}

auto forge::core::entryToRawData(const Entry& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utilxx::overload{
            [](const UMEntry& um) {
                return um.toRawData();
            },
            [](const UniqueEntry& unique) {
                return unique.toRawData();
            }},
        entry);
}

auto forge::core::entryToJson(Entry entry)
    -> Json::Value
{
    return std::visit(
        utilxx::overload{
            [](const UMEntry& um) {
                return um.toJson();
            },
            [](const UniqueEntry& unique) {
                return unique.toJson();
            }},
        entry);
}
