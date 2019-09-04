#include <entrys/Entry.hpp>
#include <entrys/EntryOperation.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <entrys/uentry/UniqueEntryOwnershipTransferOp.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOwnershipTransferOp.hpp>
#include <g3log/g3log.hpp>
#include <utilxx/Overload.hpp>
#include <variant>

using forge::core::Entry;

auto forge::core::createOwnershipTransferOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utilxx::overload{
            [](UMEntry&& um) {
                return createUMEntryOwnershipTransferOpMetadata(std::move(um));
            },
            [](UniqueEntry&& unique) {
                return createUniqueEntryOwnershipTransferOpMetadata(std::move(unique));
            },
            [](UtilityToken&& token) {
                return createUtilityTokenOwnershipTransferOpMetadata(std::move(token));
            }},
        std::move(entry));
}

auto forge::core::createRenewalOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utilxx::overload{
            [](UMEntry&& um) {
                return createUMEntryRenewalOpMetadata(std::move(um));
            },
            [](UniqueEntry&& unique) {
                return createUniqueEntryRenewalOpMetadata(std::move(unique));
            },
            [](UtilityToken&& /*unused*/) {
                LOG(WARNING) << "attemted to create a renewal operation for utility tokens";
                return std::vector<std::byte>{}; // this should never happen since tokens dont have a renewal op
            }},
        std::move(entry));
}

auto forge::core::createDeletionOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utilxx::overload{
            [](UMEntry&& um) {
                return createUMEntryDeletionOpMetadata(std::move(um));
            },
            [](UniqueEntry&& unique) {
                return createUniqueEntryDeletionOpMetadata(std::move(unique));
            },
            [](UtilityToken&& token) {
                return createUtilityTokenDeletionOpMetadata(std::move(token));
            }},
        std::move(entry));
}
