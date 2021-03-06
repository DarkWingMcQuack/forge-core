#pragma once

#include <core/Coin.hpp>
#include <core/Transaction.hpp>
#include <cstdint>
#include <client/ReadOnlyClientBase.hpp>
#include <entrys/Entry.hpp>
#include <entrys/EntryCreationOp.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <functional>
#include <lookup/UMEntryLookup.hpp>
#include <lookup/UniqueEntryLookup.hpp>
#include <lookup/UtilityTokenLookup.hpp>
#include <set>
#include <shared_mutex>
#include <utils/Opt.hpp>
#include <utils/Result.hpp>

namespace forge::lookup {

using ManagerError = std::variant<LookupError,
                                  client::ClientError>;

auto generateMessage(ManagerError&& error)
    -> std::string;

class LookupManager final
{
public:
    LookupManager(std::unique_ptr<client::ReadOnlyClientBase>&& client);
    LookupManager(LookupManager&&) = default;

    auto updateLookup()
        -> utils::Result<bool, ManagerError>;

    auto rebuildLookup()
        -> utils::Result<void, ManagerError>;

    auto lookupUMValue(const core::EntryKey& key) const
        -> utils::Opt<std::reference_wrapper<const core::UMEntryValue>>;

    auto lookupUniqueValue(const core::EntryKey& key) const
        -> utils::Opt<std::reference_wrapper<const core::UniqueEntryValue>>;

    auto lookup(const core::EntryKey& key) const
        -> utils::Opt<core::Entry>;

    auto lookupOwner(const core::EntryKey& key) const
        -> utils::Opt<std::reference_wrapper<const std::string>>;

    auto lookupActivationBlock(const core::EntryKey& key) const
        -> utils::Opt<std::reference_wrapper<const std::int64_t>>;

    auto lookupIsValid() const
        -> utils::Result<bool, client::ClientError>;

    auto getLastValidBlockHeight() const
        -> utils::Result<int64_t, client::ClientError>;

    auto getUtilityTokenCreditOf(const std::string& owner,
                                 const std::vector<std::byte>& token) const
        -> std::uint64_t;

    auto getSupplyOfToken(const std::vector<std::byte>& token) const
        -> std::uint64_t;

    auto getNumberOfExisitingTokens() const
        -> std::int64_t;

    auto getUMEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::UMEntry>;

    auto getUniqueEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::UniqueEntry>;

    auto getUtilityTokensOfOwner(const std::string& owner) const
        -> std::vector<core::UtilityToken>;

    auto getEntrysOfOwner(const std::string& owner) const
        -> std::vector<core::Entry>;

    //checks if a given key is already used as any entry key
    //or if it is free to be used  for any entry
    auto isReserverdEntryKey(const std::vector<std::byte>& key) const
        -> bool;

    auto getCoin() const
        -> core::Coin;

    auto getClient() const
        -> const client::ReadOnlyClientBase&;

private:
    auto processBlock(core::Block&& block)
        -> utils::Result<void, ManagerError>;

    auto processUMEntrys(const std::vector<core::Transaction>& txs,
                         std::int64_t block_height)
        -> void;
    auto processUniqueEntrys(const std::vector<core::Transaction>& txs,
                             std::int64_t block_height)
        -> void;
    auto processUtilityTokens(const std::vector<core::Transaction>& txs,
                              std::int64_t block_height)
        -> void;

    auto parseAndFilter(std::vector<core::Transaction>&& txs,
                        std::int64_t block_height)
        -> std::tuple<std::vector<core::UMEntryOperation>,
                      std::vector<core::UniqueEntryOperation>,
                      std::vector<core::UtilityTokenOperation>>;

    auto extractOperations(const std::vector<core::Transaction>& txs,
                           std::int64_t block_height)
        -> std::tuple<std::vector<core::UMEntryOperation>,
                      std::vector<core::UniqueEntryOperation>,
                      std::vector<core::UtilityTokenOperation>>;

    auto extractUMEntryOperations(const std::vector<core::Transaction>& txs,
                                  std::int64_t block_height)
        -> std::vector<core::UMEntryOperation>;

    auto extractUniqueEntryOperations(const std::vector<core::Transaction>& txs,
                                      std::int64_t block_height)
        -> std::vector<core::UniqueEntryOperation>;

    auto extractUtilityTokenOperations(const std::vector<core::Transaction>& txs,
                                       std::int64_t block_height)
        -> std::vector<core::UtilityTokenOperation>;

private:
    std::unique_ptr<client::ReadOnlyClientBase> client_;

    std::unique_ptr<std::shared_mutex> rw_mtx_;
    UMEntryLookup um_entry_lookup_;
    UniqueEntryLookup unique_entry_lookup_;
    UtilityTokenLookup utility_token_lookup_;
    std::int64_t lookup_block_height_;
    std::vector<std::string> block_hashes_;
};

} // namespace forge::lookup
