#pragma once

#include <json/value.h>
#include <string>
#include <utils/Opt.hpp>
#include <vector>

namespace forge::core {

class Block
{
public:
    Block(std::vector<std::string>&& txids,
          std::int64_t height,
          std::int64_t time,
          std::string&& hash);

    Block(Block&&) = default;
    Block(const Block&) = delete;

    auto operator=(Block &&)
        -> Block& = default;
    auto operator=(const Block&)
        -> Block& = default;

    auto getTxids() const
        -> const std::vector<std::string>&;
    auto getTxids()
        -> std::vector<std::string>&;

    auto getHeight() const
        -> std::int64_t;

    auto getTime() const
        -> std::int64_t;

    auto getHash() const
        -> const std::string&;
    auto getHash()
        -> std::string&;

private:
    std::vector<std::string> txids_;
    std::int64_t height_;
    std::int64_t time_;
    std::string hash_;
};

auto buildBlock(Json::Value&& json)
    -> utils::Opt<Block>;

} // namespace forge::core
