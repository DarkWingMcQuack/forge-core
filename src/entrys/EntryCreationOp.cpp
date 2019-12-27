#include <entrys/EntryCreationOp.hpp>
#include <entrys/token/UtilityTokenCreationOp.hpp>
#include <entrys/uentry/UniqueEntryCreationOp.hpp>
#include <utilxx/Overload.hpp>
#include <variant>

using forge::core::EntryCreationOp;

auto forge::core::getBurnValue(const EntryCreationOp& op)
    -> bool
{
    return std::visit(
        utilxx::overload{
            [](const core::UtilityTokenCreationOp& creation) {
                return creation.getBurnValue();
            },
            [](const core::UniqueEntryCreationOp& creation) {
                return creation.getValue();
            },
            [](const core::UMEntryCreationOp& creation) {
                return creation.getValue();
            }},
        op);
}
