#include "entrys/uentry/UniqueEntryOwnershipTransferOp.hpp"
#include "entrys/umentry/UMEntry.hpp"
#include "entrys/umentry/UMEntryOwnershipTransferOp.hpp"
#include "utilxx/Overload.hpp"
#include <entrys/Entry.hpp>
#include <entrys/EntryOperation.hpp>
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
            }},
        std::move(entry));
}
