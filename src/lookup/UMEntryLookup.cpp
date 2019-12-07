#include <algorithm>
#include <core/Coin.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <functional>
#include <g3log/g3log.hpp>
#include <lookup/LookupManager.hpp>
#include <lookup/UMEntryLookup.hpp>
#include <unordered_map>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using utilxx::Opt;
using forge::core::UMEntryOperation;
using forge::core::getValue;
using forge::core::UMEntryValue;
using forge::core::EntryKey;
using forge::core::UMEntry;
using forge::core::UMEntryCreationOp;
using forge::core::UMEntryUpdateOp;
using forge::core::UMEntryOwnershipTransferOp;
using forge::core::UMEntryRenewalOp;
using forge::core::UMEntryDeletionOp;
using forge::lookup::UMEntryLookup;


UMEntryLookup::UMEntryLookup(const LookupManager* const manager,
                             std::int64_t start_block)
    : manager_(manager),
      block_height_(start_block),
      start_block_(start_block){};


auto UMEntryLookup::executeOperations(std::vector<UMEntryOperation>&& ops)
    -> void
{
    ops = filterNonRelevantOperations(std::move(ops));

    for(auto&& op : ops) {
        std::visit(*this,
                   std::move(op));
    }
}

auto UMEntryLookup::lookup(const EntryKey& key) const
    -> Opt<std::reference_wrapper<const UMEntryValue>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<0>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookup(const EntryKey& key)
    -> Opt<std::reference_wrapper<UMEntryValue>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<0>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupOwner(const EntryKey& key) const
    -> Opt<std::reference_wrapper<const std::string>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<1>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupOwner(const EntryKey& key)
    -> Opt<std::reference_wrapper<std::string>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<1>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupUMEntry(const EntryKey& key) const
    -> utilxx::Opt<
        std::tuple<std::reference_wrapper<const core::UMEntryValue>,
                   std::reference_wrapper<const std::string>,
                   std::reference_wrapper<const std::int64_t>>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::tuple(std::cref(std::get<0>(iter->second)),
                          std::cref(std::get<1>(iter->second)),
                          std::cref(std::get<2>(iter->second)));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupUMEntry(const EntryKey& key)
    -> utilxx::Opt<
        std::tuple<std::reference_wrapper<core::UMEntryValue>,
                   std::reference_wrapper<std::string>,
                   std::reference_wrapper<std::int64_t>>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::tuple(std::ref(std::get<0>(iter->second)),
                          std::ref(std::get<1>(iter->second)),
                          std::ref(std::get<2>(iter->second)));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupActivationBlock(const core::EntryKey& key)
    -> utilxx::Opt<std::reference_wrapper<std::int64_t>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<2>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::lookupActivationBlock(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<2>(iter->second));
    }

    return std::nullopt;
}

auto UMEntryLookup::setBlockHeight(std::int64_t height)
    -> void
{
    block_height_ = height;
}


auto UMEntryLookup::removeUMEntrysOlderThan(std::int64_t diff)
    -> void
{
    auto iter = lookup_map_.begin();
    auto end_iter = lookup_map_.end();

    auto predicate = [this, &diff](auto map_iter) {
        auto activation_block = std::get<2>(map_iter->second);
        return activation_block + diff < block_height_;
    };

    //uhhhggg
    for(; iter != end_iter;) {
        if(predicate(iter)) {
            lookup_map_.erase(iter++);
        } else {
            ++iter;
        }
    }
}

auto UMEntryLookup::isCurrentlyValid(const UMEntryOperation& op) const
    -> bool
{
    const auto& op_key = std::visit(
        [](const auto& op)
            -> const core::EntryKey& {
            return op.getEntryKey();
        },
        op);


    const auto iter = lookup_map_.find(op_key);

    //if an entry with key_op does not exist and does not exist in the manager
    //then the op musst be an entry creation
    if(iter == lookup_map_.end()) {
        if(manager_ != nullptr) {
            if(!manager_->isReserverdEntryKey(op_key)) {
                return std::holds_alternative<UMEntryCreationOp>(op);
            }
            return false;
        }
        return std::holds_alternative<UMEntryCreationOp>(op);
    }

    //here an entry with the key_op already exists
    //we return true if the owner of op is the actual owner
    //of the entry currently in the lookup map
    return lookupOwner(op_key)
        .map([&op](auto looked_up_owner) {
            const auto& op_owner = std::visit(
                [](const auto& op)
                    -> const std::string& {
                    return op.getOwner();
                },
                op);

            return op_owner == looked_up_owner.get();
        })
        .valueOr(false);
}

auto UMEntryLookup::filterNonRelevantOperations(std::vector<UMEntryOperation>&& ops) const
    -> std::vector<UMEntryOperation>
{
    //erase non valid operations
    ops.erase(
        std::remove_if(std::begin(ops),
                       std::end(ops),
                       [this](const auto& op) {
                           return !isCurrentlyValid(op);
                       }),
        ops.end());

    std::map<core::EntryKey,
             std::vector<UMEntryOperation>>
        bucket_map;

    //fill map with operations
    for(auto&& op : ops) {
        auto key = std::visit(
            [](const auto& op)
                -> const EntryKey& {
                return op.getEntryKey();
            },
            op);

        if(auto iter = bucket_map.find(key);
           iter != bucket_map.end()) {
            iter->second.push_back(std::move(op));
        } else {
            bucket_map.insert({key, {std::move(op)}});
        }
    }

    std::vector<UMEntryOperation> relevant_ops;
    for(auto&& [_, operations] : bucket_map) {
        //its save to asume that there is a max element, because those
        //vectors cannot be empty
        auto max_iter =
            std::max_element(std::begin(operations),
                             std::end(operations),
                             [](const auto& lhs, const auto& rhs) {
                                 auto lhs_value = getValue(lhs);
                                 auto rhs_value = getValue(rhs);

                                 return lhs_value < rhs_value;
                             });

        //if there are two or more operations
        //with the same burn value which is the highest one
        //we dont execute any operation of them because
        //it would be ambiguous
        auto number_of_ops_with_max_burn =
            std::count_if(std::cbegin(operations),
                          std::cend(operations),
                          [&max_iter](const auto& op) {
                              return getValue(op) >= getValue(max_iter);
                          });

        if(number_of_ops_with_max_burn == 1) {
            relevant_ops.emplace_back(std::move(*max_iter));
        }
    }

    return relevant_ops;
}

auto UMEntryLookup::getUMEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::UMEntry>
{
    std::vector<core::UMEntry> ret_vec;
    utilxx::transform_if(
        std::cbegin(lookup_map_),
        std::cend(lookup_map_),
        std::back_inserter(ret_vec),
        [](const auto& value) {
            return UMEntry{value.first,
                           std::get<0>(value.second)};
        },
        [&owner](const auto& value) {
            const auto& value_owner = std::get<1>(value.second);

            return value_owner == owner;
        });

    return ret_vec;
}

auto UMEntryLookup::operator()(UMEntryCreationOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUMEntryValue());
    auto block = op.getBlock();

    //if there is no entry with this key,
    //add it
    if(auto lookup_opt = lookup(key);
       !lookup_opt) {
        auto value_tuple = std::make_tuple(std::move(value),
                                           std::move(owner),
                                           std::move(block));
        lookup_map_.insert({std::move(key),
                            std::move(value_tuple)});
    }

    LOG(DEBUG) << "executed entry creation op";
}

auto UMEntryLookup::operator()(UMEntryRenewalOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUMEntryValue());
    auto new_block = op.getBlock();

    lookupUMEntry(key)
        .onValue([&new_block,
                  &value,
                  &owner](auto pair) {
            auto [looked_value_ref,
                  looked_owner_ref,
                  looked_block_ref] = std::move(pair);

            if(looked_owner_ref.get() == owner
               && looked_value_ref.get() == value) {
                looked_block_ref.get() = new_block;
            }
        });

    LOG(DEBUG) << "executed entry renewal op";
}

auto UMEntryLookup::operator()(UMEntryOwnershipTransferOp&& op)
    -> void
{
    auto old_owner = std::move(op.getOwner());
    auto new_owner = std::move(op.getNewOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUMEntryValue());

    lookupUMEntry(key)
        .onValue([&new_owner,
                  &old_owner,
                  &value](auto entry) {
            auto [looked_value_ref,
                  looked_owner_ref,
                  _] = std::move(entry);

            if(looked_value_ref.get() == value
               && looked_owner_ref.get() == old_owner) {
                looked_owner_ref.get() = new_owner;
            }
        });

    LOG(DEBUG) << "executed ownership transfer op";
}

auto UMEntryLookup::operator()(UMEntryUpdateOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto new_value = std::move(op.getNewUMEntryValue());

    lookupUMEntry(key)
        .onValue([&owner,
                  &new_value](auto entry) {
            auto [looked_value_ref,
                  looked_owner_ref,
                  _] = std::move(entry);

            if(looked_owner_ref.get() == owner) {
                looked_value_ref.get() = new_value;
            }
        });

    LOG(DEBUG) << "executed entry update op";
}

auto UMEntryLookup::operator()(UMEntryDeletionOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUMEntryValue());

    lookupUMEntry(key)
        .onValue([&value,
                  &owner,
                  &key,
                  this](auto pair) {
            auto [looked_value_ref,
                  looked_owner_ref,
                  _] = std::move(pair);

            if(looked_owner_ref.get() == owner
               && looked_value_ref.get() == value) {
                lookup_map_.erase(key);
            }
        });

    LOG(DEBUG) << "executed entry deletion op";
}

auto UMEntryLookup::getBlockHeight() const
    -> std::int64_t
{
    return block_height_;
}

auto UMEntryLookup::clear()
    -> void
{
    lookup_map_.clear();
    block_height_ = start_block_;
}
