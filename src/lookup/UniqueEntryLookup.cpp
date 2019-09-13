#include <core/Coin.hpp>
#include <cstddef>
#include <entrys/uentry/UniqueEntryOperation.hpp>
#include <functional>
#include <g3log/g3log.hpp>
#include <lookup/LookupManager.hpp>
#include <lookup/UniqueEntryLookup.hpp>
#include <unordered_map>
#include <utilxx/Algorithm.hpp>
#include <utilxx/Opt.hpp>
#include <utilxx/Result.hpp>

using utilxx::Opt;
using forge::core::UniqueEntryOperation;
using forge::core::getValue;
using forge::core::UniqueEntryValue;
using forge::core::EntryKey;
using forge::core::UniqueEntry;
using forge::core::UniqueEntryCreationOp;
using forge::core::UniqueEntryOwnershipTransferOp;
using forge::core::UniqueEntryRenewalOp;
using forge::core::UniqueEntryDeletionOp;
using forge::lookup::UniqueEntryLookup;

UniqueEntryLookup::UniqueEntryLookup(const LookupManager* const manager,
                                     std::int64_t start_block)
    : manager_(manager),
      block_height_(start_block),
      start_block_(start_block){};

auto UniqueEntryLookup::executeOperations(std::vector<UniqueEntryOperation>&& ops)
    -> void
{
    ops = filterNonRelevantOperations(std::move(ops));

    for(auto&& op : ops) {
        std::visit(*this,
                   std::move(op));
    }
}

auto UniqueEntryLookup::lookup(const EntryKey& key) const
    -> Opt<std::reference_wrapper<const UniqueEntryValue>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<0>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::lookup(const EntryKey& key)
    -> Opt<std::reference_wrapper<UniqueEntryValue>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<0>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::lookupOwner(const EntryKey& key) const
    -> Opt<std::reference_wrapper<const std::string>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<1>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::lookupOwner(const EntryKey& key)
    -> Opt<std::reference_wrapper<std::string>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<1>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::lookupUniqueEntry(const EntryKey& key) const
    -> utilxx::Opt<
        std::tuple<std::reference_wrapper<const core::UniqueEntryValue>,
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

auto UniqueEntryLookup::lookupUniqueEntry(const EntryKey& key)
    -> utilxx::Opt<
        std::tuple<std::reference_wrapper<core::UniqueEntryValue>,
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

auto UniqueEntryLookup::lookupActivationBlock(const core::EntryKey& key)
    -> utilxx::Opt<std::reference_wrapper<std::int64_t>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::ref(std::get<2>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::lookupActivationBlock(const core::EntryKey& key) const
    -> utilxx::Opt<std::reference_wrapper<const std::int64_t>>
{
    if(auto iter = lookup_map_.find(key);
       iter != lookup_map_.end()) {
        return std::cref(std::get<2>(iter->second));
    }

    return std::nullopt;
}

auto UniqueEntryLookup::setBlockHeight(std::int64_t height)
    -> void
{
    block_height_ = height;
}


auto UniqueEntryLookup::removeUniqueEntrysOlderThan(std::int64_t diff)
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

auto UniqueEntryLookup::isCurrentlyValid(const UniqueEntryOperation& op) const
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
                return std::holds_alternative<UniqueEntryCreationOp>(op);
            }
            return false;
        }
        return std::holds_alternative<UniqueEntryCreationOp>(op);
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

auto UniqueEntryLookup::filterNonRelevantOperations(std::vector<UniqueEntryOperation>&& ops) const
    -> std::vector<UniqueEntryOperation>
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
             std::vector<UniqueEntryOperation>>
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

    std::vector<UniqueEntryOperation> relevant_ops;
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

        relevant_ops.push_back(std::move(*max_iter));
    }

    return relevant_ops;
}

auto UniqueEntryLookup::getUniqueEntrysOfOwner(const std::string& owner) const
    -> std::vector<core::UniqueEntry>
{
    std::vector<core::UniqueEntry> ret_vec;
    utilxx::transform_if(
        std::cbegin(lookup_map_),
        std::cend(lookup_map_),
        std::back_inserter(ret_vec),
        [](const auto& value) {
            return UniqueEntry{value.first,
                               std::get<0>(value.second)};
        },
        [&owner](const auto& value) {
            const auto& value_owner = std::get<1>(value.second);

            return value_owner == owner;
        });

    return ret_vec;
}

auto UniqueEntryLookup::operator()(UniqueEntryCreationOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUniqueEntryValue());
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

auto UniqueEntryLookup::operator()(UniqueEntryRenewalOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUniqueEntryValue());
    auto new_block = op.getBlock();

    lookupUniqueEntry(key)
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

auto UniqueEntryLookup::operator()(UniqueEntryOwnershipTransferOp&& op)
    -> void
{
    auto old_owner = std::move(op.getOwner());
    auto new_owner = std::move(op.getNewOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUniqueEntryValue());

    lookupUniqueEntry(key)
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

auto UniqueEntryLookup::operator()(UniqueEntryDeletionOp&& op)
    -> void
{
    auto owner = std::move(op.getOwner());
    auto key = std::move(op.getEntryKey());
    auto value = std::move(op.getUniqueEntryValue());

    lookupUniqueEntry(key)
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

auto UniqueEntryLookup::getBlockHeight() const
    -> std::int64_t
{
    return block_height_;
}

auto UniqueEntryLookup::clear()
    -> void
{
    lookup_map_.clear();
    block_height_ = start_block_;
}
