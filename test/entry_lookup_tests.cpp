#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <lookup/EntryLookup.hpp>

using namespace forge::core;
using namespace forge::lookup;

auto createOp(std::string&& data,
              std::string&& owner,
              std::int64_t block,
              std::int64_t value)
    -> Operation
{
    auto metadata =
        extractMetadata(std::move(data))
            .getValue();

    return parseMetadata(std::move(metadata),
                         block,
                         std::move(owner),
                         value)
        .getValue();
}


TEST(EntryLookupTest, BasicEntryCreationTest)
{
    std::vector ops{createOp("6a00c6dc750101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc750101aabbccdddeadbeef",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             9),
                    createOp("6a00c6dc7501040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             9)};
    EntryLookup lookup{0};

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
    EXPECT_EQ(EntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    auto second_key = stringToByteVec("0011223344").getValue();
    forge::core::NoneValue expected2;

    ASSERT_TRUE(lookup.lookupOwner(second_key));

    EXPECT_EQ("oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
              lookup.lookupOwner(second_key).getValue().get());
    EXPECT_EQ(EntryValue{expected2},
              lookup.lookup(second_key).getValue().get());

    //entry update
    ops.clear();
    ops = {createOp("6a00c6dc750801ffffffffdeadbeef",
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
    EXPECT_EQ(EntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc751001ffffffffdeadbeef",
                    "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                    12,
                    12)};

    lookup.executeOperations(std::move(ops));

    ASSERT_FALSE(lookup.lookupOwner(first_key));
}

TEST(EntryLookupTest, BasicEntryDeletionTest)
{
    std::vector ops{createOp("6a00c6dc750101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc7501040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             10)};
    EntryLookup lookup{0};

    lookup.executeOperations(std::move(ops));

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc751001aabbccdddeadbeef",
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
    EXPECT_EQ(EntryValue{expected2},
              lookup.lookup(second_key).getValue().get());
}

TEST(EntryLookupTest, BasicEntryUpdateTest)
{
    std::vector ops{createOp("6a00c6dc750101aabbccdddeadbeef",
                             "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                             10,
                             10),
                    createOp("6a00c6dc7501040011223344",
                             "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
                             10,
                             10)};
    EntryLookup lookup{0};

    lookup.executeOperations(std::move(ops));

    //entry deletion
    ops.clear();
    ops = {createOp("6a00c6dc750801ffffffffdeadbeef",
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
    EXPECT_EQ(EntryValue{expected1},
              lookup.lookup(first_key).getValue().get());

    auto second_key = stringToByteVec("0011223344").getValue();
    forge::core::NoneValue expected2;

    ASSERT_TRUE(lookup.lookupOwner(first_key));

    EXPECT_EQ("oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W",
              lookup.lookupOwner(second_key).getValue().get());
    EXPECT_EQ(EntryValue{expected2},
              lookup.lookup(second_key).getValue().get());
}
