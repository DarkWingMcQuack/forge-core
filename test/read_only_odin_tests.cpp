#include "parsing.hpp"
#include <client/odin/ReadOnlyOdinClient.hpp>
#include <fmt/core.h>
#include <gtest/gtest.h>
#include <json/value.h>

TEST(ReadOnlyOdinClientTest, processGetBlockHashValid)
{
    auto json_str1 = readFile("block_hash_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetBlockHashResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
}

TEST(ReadOnlyOdinClientTest, processGetBlockHashInvalid)
{
    auto json_str1 = readFile("block_hash_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetBlockHashResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasError());

    auto json_str2 = readFile("block_hash_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = forge::client::odin::processGetBlockHashResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasError());

    auto json_str3 = readFile("block_hash_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = forge::client::odin::processGetBlockHashResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasError());

    auto json_str4 = readFile("block_hash_invalid4.json");
    auto json4 = parseString(json_str4);

    auto res_4 = forge::client::odin::processGetBlockHashResponse(std::move(json4), {});
    ASSERT_TRUE(res_4.hasError());
}

TEST(ReadOnlyOdinClientTest, processGetBlockCountValid)
{
    auto json_str1 = readFile("block_count_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetBlockCountResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
}

TEST(ReadOnlyOdinClientTest, processGetBlockCountInvalid)
{
    auto json_str1 = readFile("block_count_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetBlockCountResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasError());

    auto json_str2 = readFile("block_count_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = forge::client::odin::processGetBlockCountResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasError());

    auto json_str3 = readFile("block_count_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = forge::client::odin::processGetBlockCountResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasError());

    auto json_str4 = readFile("block_count_invalid4.json");
    auto json4 = parseString(json_str4);

    auto res_4 = forge::client::odin::processGetBlockCountResponse(std::move(json4), {});
    ASSERT_TRUE(res_4.hasError());
}

TEST(ReadOnlyOdinClientTest, processGetUnspentValid)
{
    auto json_str1 = readFile("unspent_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetUnspentResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
    EXPECT_EQ(res_1.getValue().size(), 1);

    auto json_str2 = readFile("unspent_valid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = forge::client::odin::processGetUnspentResponse(std::move(json2), {});
    ASSERT_TRUE(res_2.hasValue());
    EXPECT_EQ(res_2.getValue().size(), 3);


    auto json_str3 = readFile("unspent_valid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = forge::client::odin::processGetUnspentResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasValue());
    EXPECT_EQ(res_3.getValue().size(), 0);
}

TEST(ReadOnlyOdinClientTest, processGetUnspentInvalid)
{
    auto json_str1 = readFile("unspent_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetUnspentResponse(std::move(json1), {});
    ASSERT_TRUE(res_1.hasValue());
    EXPECT_EQ(res_1.getValue().size(), 0);

    auto json_str2 = readFile("unspent_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = forge::client::odin::processGetUnspentResponse(std::move(json2), {});
    ASSERT_FALSE(res_2.hasValue());


    auto json_str3 = readFile("unspent_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = forge::client::odin::processGetUnspentResponse(std::move(json3), {});
    ASSERT_TRUE(res_3.hasValue());
    EXPECT_EQ(res_3.getValue().size(), 0);
}

TEST(ReadOnlyOdinClientTest, processGetAddressesValid)
{
    auto json_str1 = readFile("addressgroupings_valid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetAddressesResponse(std::move(json1));

    std::vector<std::string> expected{
        "oKXjCUKvVjqNazBc8M8ZJ1hKteFMkfFUr9",
        "oa23th4svZxotCKPG3EkpQxWxU4xgZTCrE",
        "oRyjDGLgufoKfeThD8mXJDrVEG7xDYPQWn",
        "oJGwzQdzPpWKjPRDpehxgEW47P33vsywXv",
        "oaJgrnHGPC4MwNcAUssMtYeqyoGM8NuF78",
        "oJFh6vPmiZzAYH9LVgW1Yw4ziW5cyc94xV",
        "oWSuHBhPt7wKLw3Bh1wGZxuNDuggusFQ9i",
        "oRS6ZfAthJ7N7Nj6tUsne3fPyTfZhPe6xS",
        "oNNjAdekF9YzNi1L6hoBCVD818hnkNMcr5",
        "oUPE8zejV8E7Z2UrhtU27C8HPQF7bAtngZ",
        "oNdVjHohKC8BZRAdk1HYhJ51aqFpH1kUTq",
        "obbaX4iPUUU4Pw9Xq7tFCDunK3hTAjwvVQ",
        "oamDczReYAFC3n7P4SkXrbQm1tv1d42FFC",
        "oLQvQfmuz7rMw8oXKSnbfZvsaEh9A8vt2e",
        "oJJrErC9FdVfvrMUNuDzbeNPnPSJovSrYs",
        "oRNZ1RBDKD6yBT6FXbWPaoCLtqUuW9ggxA",
        "oY66PoMNTZ59qdQDfsjBSwJm3bNq7mN8PJ",
        "oYnTsA4UsCBXGTeM3uzdEaZnUtspUN8RK6",
        "oMxNQfK6RzAPuoV77AFTsww4UZZnCtC6nL",
        "oR17cQqmtE6MLpf4A2r7wWLyqsDBwAcVcu",
        "oW9dRJaQHvJM2N6MoWfp7M9LXr1RN1FGhX",
        "oPVsKr4bM5R5ku4tG1NrZ6BZmjxag3NRta",
        "oHKNcXMoyPu62v1rGXPimuFUXM68VSNpuJ",
        "oHuq8p91jF9iHXf5ji8EMfhz94h1TSrQJJ",
        "oUY9G7vjdWCjDJzYLSyrKgjZfrMoADSPE5",
        "og6JBtmAxsL5CzAANndphrEQVkkkioYhNg",
        "oNpvvfcFVVCRN2qqLYERmiJmbrHBuASyGs",
        "oHEvMEMeU9XLaSjSR4XaXcsND1Roz1cijm",
        "oNNMXHFgZZji2woxtkcq2UaaA28tBdNwKj",
        "oMaihphYZY7T9p5cSRnsq7zXxHCgdKHv62",
        "oajxj5gBcqLgxptyzm2QRjXewKL7G9hkmB",
        "oQnYi2H2ARvmmzYxNV27jHfdmgkwTgb1Y5",
        "oRdoGzKv8tCQWHk5EJwGyDvvR9EPrTqKk1",
        "oeTNdJLQmQqJk5q9BCMUwHs6u7ioY3PaBd",
        "oLWwSw9Ld6sEYAPXfkc1gjV2bXbEXPSgar",
        "oagqtwWVVGLj5rZXmT3rkaWPyUmqoAZEoS",
        "oQFWrD7jXsXFJTgsJLVpKj1XeWZFuWwyvs",
        "oSyYcnnacucCVFhWCoE4o7kJXnZ5K6PCPc"};

    ASSERT_TRUE(res_1.hasValue());
    EXPECT_EQ(res_1.getValue().size(), 38);
    EXPECT_EQ(res_1.getValue(), expected);
}

TEST(ReadOnlyOdinClientTest, processGetAddressessInvalid)
{
    auto json_str1 = readFile("addressgroupings_invalid1.json");
    auto json1 = parseString(json_str1);

    auto res_1 = forge::client::odin::processGetAddressesResponse(std::move(json1));
    ASSERT_TRUE(res_1.hasValue());
    EXPECT_TRUE(res_1.getValue().empty());

    auto json_str2 = readFile("addressgroupings_invalid2.json");
    auto json2 = parseString(json_str2);

    auto res_2 = forge::client::odin::processGetAddressesResponse(std::move(json2));
    ASSERT_TRUE(res_2.hasError());


    auto json_str3 = readFile("addressgroupings_invalid3.json");
    auto json3 = parseString(json_str3);

    auto res_3 = forge::client::odin::processGetAddressesResponse(std::move(json3));
    ASSERT_TRUE(res_3.hasValue());
    EXPECT_TRUE(res_3.getValue().empty());
}
