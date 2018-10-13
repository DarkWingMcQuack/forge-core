#include "resource_path.hpp"
#include <core/Transaction.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

auto readFile(const std::string& path)
    -> std::string
{
    std::ifstream t{resource_path + "/" + path};
    std::string str{std::istreambuf_iterator<char>(t),
                    std::istreambuf_iterator<char>()};

    return str;
}

auto parseString(const std::string& str)
    -> Json::Value
{
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        str.c_str(),
        str.c_str() + str.size(),
        &json,
        &errors);
    delete reader;

    return json;
}

TEST(TransactionTest, TxInParsingValid)
{
    auto json_str = readFile("txin_valid.json");
    auto json = parseString(json_str);

    auto txin = buddy::core::buildTxIn(std::move(json));

    ASSERT_TRUE(static_cast<bool>(txin));

    EXPECT_STREQ(txin.getValue().getTxid().c_str(),
                 "a109a924fb7a90f305881fb9c8c5bd024673456af12e3651c27668a6b79707ad");
    EXPECT_EQ(txin.getValue().getVoutIndex(), 2);
}

TEST(TransactionTest, TxInParsingInValid)
{
    auto json_str = readFile("txin_invalid1.json");
    auto json = parseString(json_str);

    auto txin = buddy::core::buildTxIn(std::move(json));

    EXPECT_FALSE(static_cast<bool>(txin));
}

TEST(TransactionTest, TxOutParsingValid)
{
    auto json1_str = readFile("txout_valid1.json");
    auto json1 = parseString(json1_str);

    auto txout1 = buddy::core::buildTxOut(std::move(json1));

    ASSERT_TRUE(txout1);

    EXPECT_EQ(txout1.getValue().numberOfAddresses(), 1);
    EXPECT_EQ(txout1.getValue().getAddresses()[0], "oMRtieu2VEDBFYKu9XNtXPxHEKWbmssD5a");
    EXPECT_EQ(txout1.getValue().getHex(), "210306bb5eac4f028255c91fa78da9c6d0df5425e75d27b688b3e944a540f5dbe2d2ac");
    EXPECT_FALSE(txout1.getValue().isOpReturnOutput());

    auto json2_str = readFile("txout_valid2.json");
    auto json2 = parseString(json2_str);

    auto txout2 = buddy::core::buildTxOut(std::move(json2));

    ASSERT_TRUE(txout2);

    ASSERT_EQ(txout2.getValue().numberOfAddresses(), 2);
    EXPECT_EQ(txout2.getValue().getAddresses()[0], "ogA4iDjm5H2HjWUmeZykhX4HhtgMAEixGb");
    EXPECT_EQ(txout2.getValue().getAddresses()[1], "oMRtieu2VEDBFYKu9XNtXPxHEKWbmssD5a");
    EXPECT_EQ(txout2.getValue().getHex(), "76a914ff73567e44b989b5513ed0ac196d29691c43e08488ac");
    EXPECT_FALSE(txout2.getValue().isOpReturnOutput());

    auto json3_str = readFile("txout_valid3.json");
    auto json3 = parseString(json3_str);

    auto txout3 = buddy::core::buildTxOut(std::move(json3));

    ASSERT_TRUE(txout3);

    ASSERT_EQ(txout3.getValue().numberOfAddresses(), 0);
    EXPECT_EQ(txout3.getValue().getHex(), "");
    EXPECT_FALSE(txout3.getValue().isOpReturnOutput());
}

TEST(TransactionTest, TxOutParsingInvalid)
{
    auto json_str1 = readFile("txout_invalid1.json");
    auto json1 = parseString(json_str1);

    auto txout1 = buddy::core::buildTxOut(std::move(json1));

    ASSERT_FALSE(txout1);

    auto json_str2 = readFile("txout_invalid2.json");
    auto json2 = parseString(json_str2);

    auto txout2 = buddy::core::buildTxOut(std::move(json2));

    ASSERT_FALSE(txout2);
}

TEST(TransactionTest, TransactionParsingValid)
{
    auto json1_str = readFile("tx_valid1.json");
    auto json1 = parseString(json1_str);

    auto tx1 = buddy::core::buildTransaction(std::move(json1));

    ASSERT_TRUE(tx1);

    EXPECT_EQ(tx1.getValue().getTxid(), "b75fbce207c339e9413387db74ec156271ffa08b9cda01b7a0943819e7559d43");
    EXPECT_FALSE(tx1.getValue().hasOpReturnOutput());
    EXPECT_TRUE(tx1.getValue().hasExactlyOneInput());
    EXPECT_EQ(tx1.getValue().getInputs().size(), 1);
    EXPECT_EQ(tx1.getValue().getOutputs().size(), 4);
}

TEST(TransactionTest, ExtractMetadataValid)
{
    auto first_valid = buddy::core::extractMetadata("6aa109a924fb7a90f305881fb9c8c5bd024673456af12e3651c27668a6b79707ad");
    auto second_valid = buddy::core::extractMetadata("6a5b00a104aad34cacc47784a5ee9f9b8f76b94a72fa5cff81cd314cb9797b456d");
    auto third_valid = buddy::core::extractMetadata("6a44d82949d528fbb3126d8bf5fd0985836b447f26bd5b2f8251745c2064cb938f");
    auto fourth_valid = buddy::core::extractMetadata("6a44d82949d528fbbf");

    std::vector<std::byte> first_expected{
        (std::byte)0x09,
        (std::byte)0xa9,
        (std::byte)0x24,
        (std::byte)0xfb,
        (std::byte)0x7a,
        (std::byte)0x90,
        (std::byte)0xf3,
        (std::byte)0x05,
        (std::byte)0x88,
        (std::byte)0x1f,
        (std::byte)0xb9,
        (std::byte)0xc8,
        (std::byte)0xc5,
        (std::byte)0xbd,
        (std::byte)0x02,
        (std::byte)0x46,
        (std::byte)0x73,
        (std::byte)0x45,
        (std::byte)0x6a,
        (std::byte)0xf1,
        (std::byte)0x2e,
        (std::byte)0x36,
        (std::byte)0x51,
        (std::byte)0xc2,
        (std::byte)0x76,
        (std::byte)0x68,
        (std::byte)0xa6,
        (std::byte)0xb7,
        (std::byte)0x97,
        (std::byte)0x07,
        (std::byte)0xad};
    std::vector<std::byte> second_expected{
        (std::byte)0x00,
        (std::byte)0xa1,
        (std::byte)0x04,
        (std::byte)0xaa,
        (std::byte)0xd3,
        (std::byte)0x4c,
        (std::byte)0xac,
        (std::byte)0xc4,
        (std::byte)0x77,
        (std::byte)0x84,
        (std::byte)0xa5,
        (std::byte)0xee,
        (std::byte)0x9f,
        (std::byte)0x9b,
        (std::byte)0x8f,
        (std::byte)0x76,
        (std::byte)0xb9,
        (std::byte)0x4a,
        (std::byte)0x72,
        (std::byte)0xfa,
        (std::byte)0x5c,
        (std::byte)0xff,
        (std::byte)0x81,
        (std::byte)0xcd,
        (std::byte)0x31,
        (std::byte)0x4c,
        (std::byte)0xb9,
        (std::byte)0x79,
        (std::byte)0x7b,
        (std::byte)0x45,
        (std::byte)0x6d};
    std::vector<std::byte> third_expected{
        (std::byte)0xd8,
        (std::byte)0x29,
        (std::byte)0x49,
        (std::byte)0xd5,
        (std::byte)0x28,
        (std::byte)0xfb,
        (std::byte)0xb3,
        (std::byte)0x12,
        (std::byte)0x6d,
        (std::byte)0x8b,
        (std::byte)0xf5,
        (std::byte)0xfd,
        (std::byte)0x09,
        (std::byte)0x85,
        (std::byte)0x83,
        (std::byte)0x6b,
        (std::byte)0x44,
        (std::byte)0x7f,
        (std::byte)0x26,
        (std::byte)0xbd,
        (std::byte)0x5b,
        (std::byte)0x2f,
        (std::byte)0x82,
        (std::byte)0x51,
        (std::byte)0x74,
        (std::byte)0x5c,
        (std::byte)0x20,
        (std::byte)0x64,
        (std::byte)0xcb,
        (std::byte)0x93,
        (std::byte)0x8f};
    std::vector<std::byte> fourth_expected{
        (std::byte)0xd8,
        (std::byte)0x29,
        (std::byte)0x49,
        (std::byte)0xd5,
        (std::byte)0x28,
        (std::byte)0xfb,
        (std::byte)0xbf};

    ASSERT_TRUE(first_valid);
    ASSERT_TRUE(second_valid);
    ASSERT_TRUE(third_valid);
    ASSERT_TRUE(fourth_valid);

    EXPECT_EQ(first_valid.getValue(), first_expected);
    EXPECT_EQ(second_valid.getValue(), second_expected);
    EXPECT_EQ(third_valid.getValue(), third_expected);
    EXPECT_EQ(fourth_valid.getValue(), fourth_expected);
}

TEST(TransactionTest, ExtractMetadataInalid)
{
    //doesnt start with 6a
    auto first_invalid = buddy::core::extractMetadata("a109a924fb7a90f305881fb9c8c5bd024673456af12e3651c27668a6b79707ad");
    //not eeven character count
    auto second_invalid = buddy::core::extractMetadata("6a5b00a104aad34cacc47784a5ee9f9b8f76b94a72fa5cff81cd314cb9797b456");
    //bad characters
    auto third_invalid = buddy::core::extractMetadata("6a44d82ü49d528fbb3126d8bf5fd0985836b447f26bd5b2f8251745c2064cb938f");
    auto fourth_invalid = buddy::core::extractMetadata("6a44d8p949d528fbbf");
    //not enought chars
    auto fifth_invalid = buddy::core::extractMetadata("6a4");

    ASSERT_FALSE(first_invalid);
    ASSERT_FALSE(second_invalid);
    ASSERT_FALSE(third_invalid);
    ASSERT_FALSE(fourth_invalid);
    ASSERT_FALSE(fifth_invalid);
}

TEST(TransactionTest, StringToByteVecValid)
{
    auto first_valid = buddy::core::stringToByteVec("a109A924fb7a90f305881fb9c8c5bd024673456af12e3651c27668a6B79707ad");
    auto second_valid = buddy::core::stringToByteVec("5b00a104aad34cacc47784a5ee9f9b8f76b94a72fa5cfF81cd314cB9797b456d");
    auto third_valid = buddy::core::stringToByteVec("44d82949d528fbb3126d8bf5fd0985836b447f26bd5b2F8251745c2064cb938f");
    auto fourth_valid = buddy::core::stringToByteVec("44d82949d528fbBf");

    std::vector<std::byte> first_expected{
        (std::byte)0xA1,
        (std::byte)0x09,
        (std::byte)0xa9,
        (std::byte)0x24,
        (std::byte)0xfb,
        (std::byte)0x7a,
        (std::byte)0x90,
        (std::byte)0xf3,
        (std::byte)0x05,
        (std::byte)0x88,
        (std::byte)0x1f,
        (std::byte)0xb9,
        (std::byte)0xc8,
        (std::byte)0xc5,
        (std::byte)0xbd,
        (std::byte)0x02,
        (std::byte)0x46,
        (std::byte)0x73,
        (std::byte)0x45,
        (std::byte)0x6a,
        (std::byte)0xf1,
        (std::byte)0x2e,
        (std::byte)0x36,
        (std::byte)0x51,
        (std::byte)0xc2,
        (std::byte)0x76,
        (std::byte)0x68,
        (std::byte)0xa6,
        (std::byte)0xb7,
        (std::byte)0x97,
        (std::byte)0x07,
        (std::byte)0xad};
    std::vector<std::byte> second_expected{
        (std::byte)0x5b,
        (std::byte)0x00,
        (std::byte)0xa1,
        (std::byte)0x04,
        (std::byte)0xaa,
        (std::byte)0xd3,
        (std::byte)0x4c,
        (std::byte)0xac,
        (std::byte)0xc4,
        (std::byte)0x77,
        (std::byte)0x84,
        (std::byte)0xa5,
        (std::byte)0xee,
        (std::byte)0x9f,
        (std::byte)0x9b,
        (std::byte)0x8f,
        (std::byte)0x76,
        (std::byte)0xb9,
        (std::byte)0x4a,
        (std::byte)0x72,
        (std::byte)0xfa,
        (std::byte)0x5c,
        (std::byte)0xff,
        (std::byte)0x81,
        (std::byte)0xcd,
        (std::byte)0x31,
        (std::byte)0x4c,
        (std::byte)0xb9,
        (std::byte)0x79,
        (std::byte)0x7b,
        (std::byte)0x45,
        (std::byte)0x6d};
    std::vector<std::byte> third_expected{
        (std::byte)0x44,
        (std::byte)0xd8,
        (std::byte)0x29,
        (std::byte)0x49,
        (std::byte)0xd5,
        (std::byte)0x28,
        (std::byte)0xfb,
        (std::byte)0xb3,
        (std::byte)0x12,
        (std::byte)0x6d,
        (std::byte)0x8b,
        (std::byte)0xf5,
        (std::byte)0xfd,
        (std::byte)0x09,
        (std::byte)0x85,
        (std::byte)0x83,
        (std::byte)0x6b,
        (std::byte)0x44,
        (std::byte)0x7f,
        (std::byte)0x26,
        (std::byte)0xbd,
        (std::byte)0x5b,
        (std::byte)0x2f,
        (std::byte)0x82,
        (std::byte)0x51,
        (std::byte)0x74,
        (std::byte)0x5c,
        (std::byte)0x20,
        (std::byte)0x64,
        (std::byte)0xcb,
        (std::byte)0x93,
        (std::byte)0x8f};
    std::vector<std::byte> fourth_expected{
        (std::byte)0x44,
        (std::byte)0xd8,
        (std::byte)0x29,
        (std::byte)0x49,
        (std::byte)0xd5,
        (std::byte)0x28,
        (std::byte)0xfb,
        (std::byte)0xbf};

    ASSERT_TRUE(first_valid);
    ASSERT_TRUE(second_valid);
    ASSERT_TRUE(third_valid);
    ASSERT_TRUE(fourth_valid);

    EXPECT_EQ(first_valid.getValue(), first_expected);
    EXPECT_EQ(second_valid.getValue(), second_expected);
    EXPECT_EQ(third_valid.getValue(), third_expected);
    EXPECT_EQ(fourth_valid.getValue(), fourth_expected);
}

TEST(TransactionTest, StringToByteVecInvalid)
{
    //not even characters
    auto first_invalid = buddy::core::stringToByteVec("109a924fb7a90f305881fb9c8c5bd024673456af12e3651c27668a6b79707ad");
    //wrong chracters
    auto second_invalid = buddy::core::stringToByteVec("gb00a104aad34cacc47784a5ee9f9b8f76b94a72fa5cff81cd314cb9797b456d");
    auto third_invalid = buddy::core::stringToByteVec("4üd82949d528fbb3126d8bf5fd0985836b447f26bd5b2f8251745c2064cb938f");
    auto fourth_invalid = buddy::core::stringToByteVec("j4d82949d528fbbf");

    EXPECT_FALSE(first_invalid);
    EXPECT_FALSE(second_invalid);
    EXPECT_FALSE(third_invalid);
    EXPECT_FALSE(fourth_invalid);
}
