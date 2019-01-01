#include "parsing.hpp"
#include <core/Transaction.hpp>
#include <daemon/odin/ReadWriteOdinDaemon.hpp>
#include <gtest/gtest.h>
#include <json/value.h>

TEST(ReadWriteOdinDaemonTest, processGenerateRawTxResponseValid)
{
    using namespace std::string_literals;
    using buddy::core::stringToByteVec;

    auto response1 = "6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635"s;
    auto res_1 = buddy::daemon::odin::processGenerateRawTxResponse(response1);
    ASSERT_TRUE(res_1.hasValue());

    auto expected1 =
        stringToByteVec("6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635")
            .getValue();
    EXPECT_EQ(res_1.getValue(), expected1);


    auto response2 = "e1f0d6373e51c572df26f19e161bc0463978604c1148fb4d66fe7800e8c66f31"s;
    auto res_2 = buddy::daemon::odin::processGenerateRawTxResponse(response2);
    ASSERT_TRUE(res_2.hasValue());

    auto expected2 =
        stringToByteVec("e1f0d6373e51c572df26f19e161bc0463978604c1148fb4d66fe7800e8c66f31")
            .getValue();
    EXPECT_EQ(res_2.getValue(), expected2);
}

TEST(ReadWriteOdinDaemonTest, processGenerateRawTxResponseInvalid)
{
    using namespace std::string_literals;
    using buddy::core::stringToByteVec;

    auto response1 = "6X14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635"s;
    auto res_1 = buddy::daemon::odin::processGenerateRawTxResponse(response1);
    ASSERT_TRUE(res_1.hasError());

    auto res_2 = buddy::daemon::odin::processGenerateRawTxResponse(10);
    ASSERT_TRUE(res_2.hasError());
}

TEST(ReadWriteOdinDaemonTest, processSignRawTxResponseValid)
{
    using buddy::core::stringToByteVec;

    auto file1 = readFile("sign_raw_tx_valid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processSignRawTxResponse(std::move(json1));

    ASSERT_TRUE(res1.hasValue());

    auto expected =
        stringToByteVec("010000000176fdc4877639a2b1344f2b8eff0e8f574b290451d7b7ab8b714c4b8893285779010000006a47304402200f848a103d06cb4f8a7bd571314fff2c1e52a5d130ac2b7f3ee5faf5325bc6e202200b23a0a7d5a1a6348d1edd49cdbb162545641f370a008c7e0a66403079d1d306012102f285704370a212d308afd673f589e1d7de75c1f070cdef4579fbb62d23043a6affffffff01c022be1d000000000f6a0d476f6f6462796520576f726c6400000000")
            .getValue();

    EXPECT_EQ(res1.getValue(), expected);
}

TEST(ReadWriteOdinDaemonTest, processSignRawTxResponseInvalid)
{
    auto file1 = readFile("sign_raw_tx_invalid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processSignRawTxResponse(std::move(json1));

    ASSERT_TRUE(res1.hasError());


    auto file2 = readFile("sign_raw_tx_invalid2.json");
    auto json2 = parseString(file2);

    auto res2 = buddy::daemon::odin::processSignRawTxResponse(std::move(json2));

    ASSERT_TRUE(res2.hasError());

    auto file3 = readFile("sign_raw_tx_invalid3.json");
    auto json3 = parseString(file3);

    auto res3 = buddy::daemon::odin::processSignRawTxResponse(std::move(json3));

    ASSERT_TRUE(res3.hasError());
    EXPECT_STREQ(res3.getError().what(), "fuckin error");


    auto file4 = readFile("sign_raw_tx_invalid4.json");
    auto json4 = parseString(file4);

    auto res4 = buddy::daemon::odin::processSignRawTxResponse(std::move(json4));

    ASSERT_TRUE(res4.hasError());
}

TEST(ReadWriteOdinDaemonTest, processGenerateNewAddressResponseValid)
{
    auto file1 = readFile("generate_new_address_valid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processGenerateNewAddressResponse(std::move(json1));

    ASSERT_TRUE(res1.hasValue());
    EXPECT_EQ(res1.getValue(), "oRDxB5XznfHGDMPcRyjD2cnq6hahtpWkTT");
}

TEST(ReadWriteOdinDaemonTest, processGenerateNewAddressResponseInvalid)
{
    auto file1 = readFile("generate_new_address_invalid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processGenerateNewAddressResponse(std::move(json1));

    ASSERT_TRUE(res1.hasError());


    auto file2 = readFile("generate_new_address_invalid2.json");
    auto json2 = parseString(file2);

    auto res2 = buddy::daemon::odin::processGenerateNewAddressResponse(std::move(json2));

    ASSERT_TRUE(res2.hasError());
}

TEST(ReadWriteOdinDaemonTest, processDecodeTxidOfRawTxResponseValid)
{
    auto file1 = readFile("decode_txid_valid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processDecodeTxidOfRawTxResponse(std::move(json1));

    ASSERT_TRUE(res1.hasValue());
    EXPECT_EQ(res1.getValue(), "6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635");
}

TEST(ReadWriteOdinDaemonTest, processDecodeTxidOfRawTxResponseInvalid)
{
    auto file1 = readFile("decode_txid_invalid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processDecodeTxidOfRawTxResponse(std::move(json1));

    ASSERT_TRUE(res1.hasError());


    auto file2 = readFile("decode_txid_invalid2.json");
    auto json2 = parseString(file2);

    auto res2 = buddy::daemon::odin::processGenerateNewAddressResponse(std::move(json2));

    ASSERT_TRUE(res2.hasError());
}


TEST(ReadWriteOdinDaemonTest, processSendToAddressResponseValid)
{
    using buddy::core::stringToByteVec;

    auto file1 = readFile("send_to_address_valid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processSendToAddressResponse(std::move(json1),
                                                                  {});

    ASSERT_TRUE(res1.hasValue());

    EXPECT_EQ(res1.getValue(), "6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635");
}

TEST(ReadWriteOdinDaemonTest, processSendToAddressResponseInvalid)
{
    auto file1 = readFile("send_to_address_invalid1.json");
    auto json1 = parseString(file1);

    auto res1 = buddy::daemon::odin::processSendToAddressResponse(std::move(json1),
                                                                  {});

    ASSERT_TRUE(res1.hasError());


    auto file2 = readFile("send_to_address_invalid2.json");
    auto json2 = parseString(file2);

    auto res2 = buddy::daemon::odin::processSendToAddressResponse(std::move(json1),
                                                                  {});

    ASSERT_TRUE(res2.hasError());
}
