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

TEST(TransactionTest, TxOutParsing)
{
}

TEST(TransactionTest, TransactionParsing)
{
}

TEST(TransactionTest, ExtractMetadata)
{
}

TEST(TransactionTest, StringToByteVec)
{
}

TEST(TransactionTest, MetadataStartsWithId)
{
}
