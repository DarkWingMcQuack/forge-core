#include <entrys/Entry.hpp>
#include <entrys/uentry/UniqueEntry.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <utils/Opt.hpp>
#include <utils/Overload.hpp>
#include <variant>
#include <vector>

using forge::core::Entry;

auto forge::core::parseEntry(const std::vector<std::byte>& data)
    -> utils::Opt<Entry>
{
    auto um_entry_opt = parseUMEntry(data);
    if(um_entry_opt) {
        return um_entry_opt
            .map([](auto entry) {
                return Entry{std::move(entry)};
            });
    }

    auto unique_entry_opt = parseUniqueEntry(data);
    if(unique_entry_opt) {
        return unique_entry_opt
            .map([](auto entry) {
                return Entry{std::move(entry)};
            });
    }

    return parseUtilityToken(data)
        .map([](auto entry) {
            return Entry{std::move(entry)};
        });
}

auto forge::core::entryToRawData(const Entry& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        [](const auto& en) {
            return en.toRawData();
        },
        entry);
}

auto forge::core::entryToJson(const Entry& entry)
    -> Json::Value
{
    return std::visit(
        [](const auto& en) {
            return en.toJson();
        },
        entry);
}
