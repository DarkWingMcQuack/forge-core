#include <algorithm>
#include <array>
#include <core/Block.hpp>
#include <cstddef>
#include <utilxx/Opt.hpp>
#include <vector>

using forge::core::Block;
using utilxx::Opt;



Block::Block(std::vector<std::string>&& txids,
             std::int64_t height,
             std::int64_t time,
             std::string&& hash)
    : txids_(std::move(txids)),
      height_(height),
      time_(time),
      hash_(std::move(hash)) {}

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
    -> std::int64_t
{
    return height_;
}

auto Block::getTime() const
    -> std::int64_t
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



auto forge::core::buildBlock(Json::Value&& json)
    -> utilxx::Opt<Block>
{
    //use try-catch such if all the json.asFoo()
    //are failing we get a nullopt
    try {
        //check if all the needed attributes are there
        if(!json.isMember("tx")
           || !json.isMember("time")
           || !json.isMember("height")
           || !json.isMember("hash")) {
            return std::nullopt;
        }

        std::vector<std::string> transactions;
        std::transform(std::cbegin(json["tx"]),
                       std::cend(json["tx"]),
                       std::back_inserter(transactions),
                       [](auto json) {
                           return json.asString();
                       });


        //extract blocktime
        auto time = json["time"].asUInt();

        //extract block height
        auto height = json["height"].asUInt();

        //extract hash
        auto hash = std::move(json["hash"].asString());

        return Block{std::move(transactions),
                     height,
                     time,
                     std::move(hash)};
    } catch(...) {
        return std::nullopt;
    }
}
