#include <core/Entry.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(EntryTest, EntryKeyParsingValidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0100000000deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff0100000000AA").getValue();
    auto entry_opt2 = buddy::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryKeyParsingValidNoneValue)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff04deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff04AA").getValue();
    auto entry_opt2 = buddy::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}


TEST(EntryTest, EntryKeyParsingValidByteArray)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0803000000deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff0810ffffffffffffffffffffffffffffffffAA").getValue();
    auto entry_opt2 = buddy::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);

    auto data3 = buddy::core::stringToByteVec("ffffffff0800AA").getValue();
    auto entry_opt3 = buddy::core::parseKey(data3);
    std::vector expected3{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt3);
    EXPECT_EQ(entry_opt3.getValue(), expected3);

    auto data4 = buddy::core::stringToByteVec("ffffffff0801CCAA").getValue();
    auto entry_opt4 = buddy::core::parseKey(data4);
    std::vector expected4{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt4);
    EXPECT_EQ(entry_opt4.getValue(), expected4);
}

TEST(EntryTest, EntryKeyParsingValidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0201010101010101010101010101010101deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff0200000000000000000000000000000000AA").getValue();
    auto entry_opt2 = buddy::core::parseKey(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryKeyParsingInvalidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0100000000").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidNoneValue)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff04").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidByteArray)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff080500000000").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryKeyParsingInvalidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0201010101010101010101010101010101").getValue();
    auto entry_opt1 = buddy::core::parseKey(data1);
    EXPECT_FALSE(entry_opt1);
}

TEST(EntryTest, EntryValueParsingValidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0100000000deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseValue(data1);
    std::array expected1{
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00,
        (std::byte)0x00};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), buddy::core::EntryValue{expected1});

    auto data2 = buddy::core::stringToByteVec("ffffffff01aabbccdddeadbeef").getValue();
    auto entry_opt2 = buddy::core::parseValue(data2);
    std::array expected2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), buddy::core::EntryValue{expected2});
}

TEST(EntryTest, EntryValueParsingValidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0200000000000000000000000000000000AA").getValue();
    auto entry_opt1 = buddy::core::parseValue(data1);
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
    EXPECT_EQ(entry_opt1.getValue(), buddy::core::EntryValue{expected1});

    auto data2 = buddy::core::stringToByteVec("ffffffff02aabbccddeeff11223344556677889900deadbeef").getValue();
    auto entry_opt2 = buddy::core::parseValue(data2);
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
    EXPECT_EQ(entry_opt2.getValue(), buddy::core::EntryValue{expected2});
}

TEST(EntryTest, EntryValueParsingInvalidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff01000000").getValue();
    auto entry_opt1 = buddy::core::parseValue(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = buddy::core::stringToByteVec("ffffffff01aabbcc").getValue();
    auto entry_opt2 = buddy::core::parseValue(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryValueParsingInvalidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff02000000000000000000000000000000").getValue();
    auto entry_opt1 = buddy::core::parseValue(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = buddy::core::stringToByteVec("ffffffff02aabbccddeeff001122334455667788").getValue();
    auto entry_opt2 = buddy::core::parseValue(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryParsingValidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0100000000deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseEntry(data1);
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
    std::pair expected1{key1, buddy::core::EntryValue{value1}};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff01aabbccddAA").getValue();
    auto entry_opt2 = buddy::core::parseEntry(data2);

    std::array value2{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};
    std::vector key2{(std::byte)0xaa};
    std::pair expected2{key2, buddy::core::EntryValue{value2}};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryParsingValidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff0210101010101010101010101010101010deadbeef").getValue();
    auto entry_opt1 = buddy::core::parseEntry(data1);
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
    std::pair expected1{key1, buddy::core::EntryValue{value1}};

    ASSERT_TRUE(entry_opt1);
    EXPECT_EQ(entry_opt1.getValue(), expected1);

    auto data2 = buddy::core::stringToByteVec("ffffffff02aabbccddeeff11223344556677889900deadbeef").getValue();
    auto entry_opt2 = buddy::core::parseEntry(data2);
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

    std::pair expected2{key1, buddy::core::EntryValue{value2}};

    ASSERT_TRUE(entry_opt2);
    EXPECT_EQ(entry_opt2.getValue(), expected2);
}

TEST(EntryTest, EntryParsingInvalidIpv6)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff02000000000000000000000000000000").getValue();
    auto entry_opt1 = buddy::core::parseEntry(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = buddy::core::stringToByteVec("ffffffff02aabbccddeeff001122334455667788").getValue();
    auto entry_opt2 = buddy::core::parseEntry(data2);

    ASSERT_FALSE(entry_opt2);
}

TEST(EntryTest, EntryParsingInvalidIpv4)
{
    auto data1 = buddy::core::stringToByteVec("ffffffff01000000").getValue();
    auto entry_opt1 = buddy::core::parseEntry(data1);

    ASSERT_FALSE(entry_opt1);

    auto data2 = buddy::core::stringToByteVec("ffffffff01aabbcc").getValue();
    auto entry_opt2 = buddy::core::parseEntry(data2);

    ASSERT_FALSE(entry_opt2);
}
