#include "parsing.hpp"
#include <daemon/odin/ReadOnlyOdinDaemon.hpp>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <json/value.h>

TEST(ReadOnlyOdinDaemonTest, processGetBlockHashValid)
{
    auto json_str1 = readFile("block_hash_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = buddy::daemon::odin::processGetBlockHashResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
}

TEST(ReadOnlyOdinDaemonTest, processGetBlockHashInvalid)
{
    auto json_str1 = readFile("block_hash_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = buddy::daemon::odin::processGetBlockHashResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasError());

    auto json_str2 = readFile("block_hash_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = buddy::daemon::odin::processGetBlockHashResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasError());

    auto json_str3 = readFile("block_hash_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = buddy::daemon::odin::processGetBlockHashResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasError());

    auto json_str4 = readFile("block_hash_invalid4.json");
    auto json4 = parseString(json_str4);

    auto res_4 = buddy::daemon::odin::processGetBlockHashResponse(std::move(json4), {});
    ASSERT_TRUE(res_4.hasError());
}

TEST(ReadOnlyOdinDaemonTest, processGetBlockCountValid)
{
    auto json_str1 = readFile("block_count_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = buddy::daemon::odin::processGetBlockCountResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
}

TEST(ReadOnlyOdinDaemonTest, processGetBlockCountInvalid)
{
    auto json_str1 = readFile("block_count_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = buddy::daemon::odin::processGetBlockCountResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasError());

    auto json_str2 = readFile("block_count_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = buddy::daemon::odin::processGetBlockCountResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasError());

    auto json_str3 = readFile("block_count_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = buddy::daemon::odin::processGetBlockCountResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasError());

    auto json_str4 = readFile("block_count_invalid4.json");
    auto json4 = parseString(json_str4);

    auto res_4 = buddy::daemon::odin::processGetBlockCountResponse(std::move(json4), {});
    ASSERT_TRUE(res_4.hasError());
}

TEST(ReadOnlyOdinDaemonTest, processGetUnspentValid)
{
    auto json_str1 = readFile("unspent_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = buddy::daemon::odin::processGetUnspentResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
    EXPECT_EQ(res_1.getValue().size(), 1);

    auto json_str2 = readFile("unspent_valid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = buddy::daemon::odin::processGetUnspentResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasValue());
    EXPECT_EQ(res_2.getValue().size(), 3);


    auto json_str3 = readFile("unspent_valid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = buddy::daemon::odin::processGetUnspentResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasValue());
    EXPECT_EQ(res_3.getValue().size(), 0);
}
