#include <array>
#include <core/Block.hpp>
#include <cstddef>
#include <vector>
#include <algorithm>

using buddy::core::Block;


Block::Block(std::vector<std::string>&& txids,
             std::size_t height,
             std::size_t time,
             std::string&& hash)
    : txids_(std::move(txids)),
      height_(height),
      time_(time),
      hash_(std::move(hash)) {}

Block::Block(Json::Value&& json,
             std::string&& hash)
    : hash_(std::move(hash))
{
    std::transform(std::cbegin(json["tx"]),
                   std::cend(json["tx"]),
                   std::back_inserter(txids_),
                   [](auto&& json) {
                       return std::move(json.asString());
                   });


    //extract blocktime
    time_ = json["time"].asUInt();
    //extract block height
    height_ = json["height"].asUInt();
}

auto Block::getTxids() const
    -> const std::vector<std::string>&
{
    return txids_;
}

auto Block::getTxids()
    -> std::vector<std::string>&
{
    return txids_;
}


auto Block::getHeight() const
    -> std::size_t
{
    return height_;
}

auto Block::getTime() const
    -> std::size_t
{
    return time_;
}

auto Block::getHash() const
    -> const std::string&
{
    return hash_;
}

auto Block::getHash()
    -> std::string&
{
    return hash_;
}
