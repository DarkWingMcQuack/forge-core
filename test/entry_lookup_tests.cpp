#include <entrys/umentry/UMEntry.hpp>
#include <entrys/umentry/UMEntryOperation.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <lookup/UMEntryLookup.hpp>

using namespace forge::core;
using namespace forge::lookup;

auto createOp(std::string&& data,
              std::string&& owner,
              std::int64_t block,
              std::int64_t value)
    -> UMEntryOperation
{
    auto metadata =
        extractMetadata(std::move(data))
            .getValue();

    return parseMetadataToUMEntryOp(std::move(metadata),
                         block,
                         std::move(owner),
                         value)
        .getValue();
}


TEST(UMEntryLookupTest, BasicUMEntryCreationTest)
{
    std::vector ops{createOp("6a00c6dc75010101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc75010101aabbccdddeadbeef",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             9),
                    createOp("6a00c6dc750101040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             9)};
    UMEntryLookup lookup{0};

    lookup.executeOperations(std::move(ops));

    auto first_key = stringToByteVec("deadbeef").getValue();
    std::array expected1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    ASSERT_TRUE(lookup.lookupOwner(first_key));

    EXPECT_EQ("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
              lookup.lookupOwner(first_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    auto second_key = stringToByteVec("0011223344").getValue();
    forge::core::NoneValue expected2;

    ASSERT_TRUE(lookup.lookupOwner(second_key));

    EXPECT_EQ("oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
              lookup.lookupOwner(second_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected2},
              lookup.lookup(second_key).getValue().get());

    //entry update
    ops.clear();
    ops = {createOp("6a00c6dc75010801ffffffffdeadbeef",
                    "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                    11,
                    11)};

    lookup.executeOperations(std::move(ops));

    expected1 = {(std::byte)0xff,
                 (std::byte)0xff,
                 (std::byte)0xff,
                 (std::byte)0xff};

    ASSERT_TRUE(lookup.lookupOwner(first_key));

    EXPECT_EQ("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
              lookup.lookupOwner(first_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc75011001ffffffffdeadbeef",
                    "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                    12,
                    12)};

    lookup.executeOperations(std::move(ops));

    ASSERT_FALSE(lookup.lookupOwner(first_key));
}

TEST(UMEntryLookupTest, BasicUMEntryDeletionTest)
{
    std::vector ops{createOp("6a00c6dc75010101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc750101040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             10)};
    UMEntryLookup lookup{0};

    lookup.executeOperations(std::move(ops));

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc75011001aabbccdddeadbeef",
                    "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                    12,
                    12)};

    lookup.executeOperations(std::move(ops));

    auto first_key = stringToByteVec("deadbeef").getValue();

    ASSERT_FALSE(lookup.lookupOwner(first_key));

    auto second_key = stringToByteVec("0011223344").getValue();
    forge::core::NoneValue expected2;

    ASSERT_TRUE(lookup.lookupOwner(second_key));

    EXPECT_EQ("oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
              lookup.lookupOwner(second_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected2},
              lookup.lookup(second_key).getValue().get());
}

TEST(UMEntryLookupTest, BasicUMEntryUpdateTest)
{
    std::vector ops{createOp("6a00c6dc75010101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc750101040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             10)};
    UMEntryLookup lookup{0};

    lookup.executeOperations(std::move(ops));

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc75010801ffffffffdeadbeef",
                    "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                    12,
                    12)};

    lookup.executeOperations(std::move(ops));

    auto first_key = stringToByteVec("deadbeef").getValue();
    std::array expected1{
        (std::byte)0xff,
        (std::byte)0xff,
        (std::byte)0xff,
        (std::byte)0xff};

    ASSERT_TRUE(lookup.lookupOwner(first_key));

    EXPECT_EQ("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
              lookup.lookupOwner(first_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    auto second_key = stringToByteVec("0011223344").getValue();
    forge::core::NoneValue expected2;

    ASSERT_TRUE(lookup.lookupOwner(first_key));

    EXPECT_EQ("oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
              lookup.lookupOwner(second_key).getValue().get());
    EXPECT_EQ(UMEntryValue{expected2},
              lookup.lookup(second_key).getValue().get());
}
