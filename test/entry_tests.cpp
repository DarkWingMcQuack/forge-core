#include <core/Entry.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(EntryTest, EntryKeyParsingValidIpv4)
{
    auto data1 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "01" //token type
                     "ff" //operation
                     "01" //value flag
                     "00000000deadbeef")
                     .getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "01" // token type
                     "ff" //operation flag
                     "01" //value flag
                     "00000000AA")
                     .getValue();
    auto entry_opt2 = forge::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryKeyParsingValidNoneValue)
{
    auto data1 = forge::core::stringToByteVec("ffffff00ff04deadbeef").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = forge::core::stringToByteVec("ffffff00ff04AA").getValue();
    auto entry_opt2 = forge::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}


TEST(EntryTest, EntryKeyParsingValidByteArray)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0803000000deadbeef").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = forge::core::stringToByteVec("ffffff01ff0810ffffffffffffffffffffffffffffffffAA").getValue();
    auto entry_opt2 = forge::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);

    auto data3 = forge::core::stringToByteVec("ffffff01ff0800AA").getValue();
    auto entry_opt3 = forge::core::parseKey(data3);
    std::vector expected3{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt3);
    EXPECT_EQ(entry_opt3.getValue(), expected3);

    auto data4 = forge::core::stringToByteVec("ffffff01ff0801CCAA").getValue();
    auto entry_opt4 = forge::core::parseKey(data4);
    std::vector expected4{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt4);
    EXPECT_EQ(entry_opt4.getValue(), expected4);
}

TEST(EntryTest, EntryKeyParsingValidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0201010101010101010101010101010101deadbeef").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = forge::core::stringToByteVec("ffffff01ff0200000000000000000000000000000000AA").getValue();
    auto entry_opt2 = forge::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryKeyParsingInvalidIpv4)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0100000000").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidNoneValue)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff04").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidByteArray)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff080500000000").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0201010101010101010101010101010101").getValue();
    auto entry_opt1 = forge::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryValueParsingValidIpv4)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0100000000deadbeef").getValue();
    auto entry_opt1 = forge::core::parseValue(data1);
    std::array expected1{
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), forge::core::EntryValue{expected1});

    auto data2 = forge::core::stringToByteVec("ffffff01ff01aabbccdddeadbeef").getValue();
    auto entry_opt2 = forge::core::parseValue(data2);
    std::array expected2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), forge::core::EntryValue{expected2});
}

TEST(EntryTest, EntryValueParsingValidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0200000000000000000000000000000000AA").getValue();
    auto entry_opt1 = forge::core::parseValue(data1);
    std::array expected1{
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), forge::core::EntryValue{expected1});

    auto data2 = forge::core::stringToByteVec("ffffff01ff02aabbccddeeff11223344556677889900deadbeef").getValue();
    auto entry_opt2 = forge::core::parseValue(data2);
    std::array expected2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd,
        (std::byte)0xee,
        (std::byte)0xff,
        (std::byte)0x11,
        (std::byte)0x22,
        (std::byte)0x33,
        (std::byte)0x44,
        (std::byte)0x55,
        (std::byte)0x66,
        (std::byte)0x77,
        (std::byte)0x88,
        (std::byte)0x99,
        (std::byte)0x00};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), forge::core::EntryValue{expected2});
}

TEST(EntryTest, EntryValueParsingInvalidIpv4)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff01000000").getValue();
    auto entry_opt1 = forge::core::parseValue(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = forge::core::stringToByteVec("ffffff01ff01aabbcc").getValue();
    auto entry_opt2 = forge::core::parseValue(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryValueParsingInvalidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff02000000000000000000000000000000").getValue();
    auto entry_opt1 = forge::core::parseValue(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = forge::core::stringToByteVec("ffffff01ff02aabbccddeeff001122334455667788").getValue();
    auto entry_opt2 = forge::core::parseValue(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryParsingValidIpv4)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0100000000deadbeef").getValue();
    auto entry_opt1 = forge::core::parseEntry(data1);
    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};
    std::array value1{
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00};
    std::pair expected1{key1, forge::core::EntryValue{value1}};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data11 = forge::core::entryToRawData(entry_opt1.getValue());
    auto expected11 = forge::core::stringToByteVec("0100000000deadbeef").getValue();
    EXPECT_EQ(data11, expected11);


    auto data2 = forge::core::stringToByteVec("ffffff01ff01aabbccddAA").getValue();
    auto entry_opt2 = forge::core::parseEntry(data2);

    std::array value2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};
    std::vector key2{(std::byte)0xaa};
    std::pair expected2{key2, forge::core::EntryValue{value2}};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);

    auto data21 = forge::core::entryToRawData(entry_opt2.getValue());
    auto expected21 = forge::core::stringToByteVec("01aabbccddAA").getValue();
    EXPECT_EQ(data21, expected21);
}

TEST(EntryTest, EntryParsingValidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff01ff0210101010101010101010101010101010deadbeef").getValue();
    auto entry_opt1 = forge::core::parseEntry(data1);
    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};
    std::array value1{
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10,
        (std::byte)0x10};
    std::pair expected1{key1, forge::core::EntryValue{value1}};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data11 = forge::core::entryToRawData(entry_opt1.getValue());
    auto expected11 = forge::core::stringToByteVec("0210101010101010101010101010101010deadbeef").getValue();
    EXPECT_EQ(data11, expected11);

    auto data2 = forge::core::stringToByteVec("ffffff00ff02aabbccddeeff11223344556677889900deadbeef").getValue();
    auto entry_opt2 = forge::core::parseEntry(data2);
    std::array value2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd,
        (std::byte)0xee,
        (std::byte)0xff,
        (std::byte)0x11,
        (std::byte)0x22,
        (std::byte)0x33,
        (std::byte)0x44,
        (std::byte)0x55,
        (std::byte)0x66,
        (std::byte)0x77,
        (std::byte)0x88,
        (std::byte)0x99,
        (std::byte)0x00};

    std::pair expected2{key1, forge::core::EntryValue{value2}};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);

    auto data21 = forge::core::entryToRawData(entry_opt1.getValue());
    auto expected21 = forge::core::stringToByteVec("02aabbccddeeff11223344556677889900deadbeef").getValue();
    EXPECT_EQ(data11, expected11);
}

TEST(EntryTest, EntryParsingInvalidIpv6)
{
    auto data1 = forge::core::stringToByteVec("ffffff00ff02000000000000000000000000000000").getValue();
    auto entry_opt1 = forge::core::parseEntry(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = forge::core::stringToByteVec("ffffff00ff02aabbccddeeff001122334455667788").getValue();
    auto entry_opt2 = forge::core::parseEntry(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryParsingInvalidIpv4)
{
    auto data1 = forge::core::stringToByteVec("ffffff00ff01000000").getValue();
    auto entry_opt1 = forge::core::parseEntry(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = forge::core::stringToByteVec("ffffff00ff01aabbcc").getValue();
    auto entry_opt2 = forge::core::parseEntry(data2);

    ASSERT_FALSE(entry_opt2);
}
