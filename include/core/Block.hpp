#pragma once

#include <json/value.h>
#include <string>
#include <util/Opt.hpp>
#include <vector>

namespace buddy::core {

class Block
{
public:
    Block(std::vector<std::string>&& txids,
          std::size_t height,
          std::size_t time,
          std::string&& hash);

    Block(Json::Value&& json,
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
        -> std::size_t;

    auto getTime() const
        -> std::size_t;

    auto getHash() const
        -> const std::string&;
    auto getHash()
        -> std::string&;

private:
    std::vector<std::string> txids_;
    std::size_t height_;
    std::size_t time_;
    std::string hash_;
};

auto buildBlock(Json::Value&& json)
    -> util::Opt<Block>;

} // namespace buddy::core
