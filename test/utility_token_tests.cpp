#include <core/Transaction.hpp>
#include <entrys/token/UtilityToken.hpp>
#include <gtest/gtest.h>
#include <iostream>

TEST(UtilityTokenTest, UtilityTokenValidParsing)
{
    auto data1 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "03" //token type
                     "ff" //operation
                     "0000000000000000" //amount
                     "deadbeef") //id
                     .getValue();
    auto token_opt1 = forge::core::parseUtilityToken(data1);

    std::vector expected1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    ASSERT_TRUE(token_opt1);
    EXPECT_EQ(token_opt1.getValue().getId(), expected1);

    auto data2 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "03" //token type
                     "ff" //operation
                     "0000000000000000" //amount
                     "AA")
                     .getValue();

    auto token_opt2 = forge::core::parseUtilityToken(data2);
    std::vector expected2{(std::byte)0xaa};

    ASSERT_TRUE(token_opt2);
    EXPECT_EQ(token_opt2.getValue().getId(), expected2);
}

TEST(UtilityTokenTest, UtilityTokenInvalidParsing)
{
    //TO LITTLE BYTES
    auto data1 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "03" //token type
                     "ff" //operation
                     "0000000000000000")
                     .getValue();
    auto token_opt1 = forge::core::parseUtilityToken(data1);
    ASSERT_FALSE(token_opt1);

    //WRONG TOKEN TYPE FLAG
    auto data2 = forge::core::stringToByteVec(
                     "ffffff" //mask
                     "01" //token type
                     "ff" //operation
                     "0000000000000000" //amount
                     "AA")
                     .getValue();

    auto token_opt2 = forge::core::parseUtilityToken(data2);

    ASSERT_FALSE(token_opt2);
}
