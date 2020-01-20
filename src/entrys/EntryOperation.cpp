#include <entrys/Entry.hpp>
#include <entrys/EntryOperation.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <entrys/uentry/UniqueEntryOwnershipTransferOp.hpp>
#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOwnershipTransferOp.hpp>
#include <g3log/g3log.hpp>
#include <utils/Overload.hpp>
#include <variant>

using forge::core::Entry;

auto forge::core::createOwnershipTransferOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utils::overload{
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

auto forge::core::createRenewalOpMetadata(RenewableEntry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utils::overload{
            [](UMEntry&& um) {
                return createUMEntryRenewalOpMetadata(std::move(um));
            },
            [](UniqueEntry&& unique) {
                return createUniqueEntryRenewalOpMetadata(std::move(unique));
            }},
        std::move(entry));
}

auto forge::core::createDeletionOpMetadata(Entry&& entry)
    -> std::vector<std::byte>
{
    return std::visit(
        utils::overload{
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
