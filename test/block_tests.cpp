#include "parsing.hpp"
#include <core/Block.hpp>
#include <gtest/gtest.h>
#include <json/value.h>

TEST(BlockTest, BlockBuildingValid)
{
    auto json_str1 = readFile("block_valid1.json");
    auto json1 = parseString(json_str1);

    auto block1 = forge::core::buildBlock(std::move(json1));

    ASSERT_TRUE(block1);

    EXPECT_EQ(block1.getValue().getHash(),
              "8fc724cddcd8a5d6f88bfcf2242eafc96a36d22707183f21d229fac18191adb7");
    EXPECT_EQ(block1.getValue().getTxids().size(), 2);
    EXPECT_EQ(block1.getValue().getTxids()[0],
              "d0febcb8eee13b5305c3ee205717fa8daa2667379283524fb8fbdae11f66d9d3");
    EXPECT_EQ(block1.getValue().getTxids()[1],
              "b75fbce207c339e9413387db74ec156271ffa08b9cda01b7a0943819e7559d43");
    EXPECT_EQ(block1.getValue().getHeight(), 20000);
    EXPECT_EQ(block1.getValue().getTime(), 1539323011);
}

TEST(BlockTest, BlockBuildingInvalid)
{
    auto json_str1 = readFile("block_invalid1.json");
    auto json1 = parseString(json_str1);

    auto block1 = forge::core::buildBlock(std::move(json1));

    ASSERT_FALSE(block1);

    auto json_str2 = readFile("block_invalid2.json");
    auto json2 = parseString(json_str2);

    auto block2 = forge::core::buildBlock(std::move(json2));

    ASSERT_FALSE(block2);
}
