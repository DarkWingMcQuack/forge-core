#include <core/Entry.hpp>
#include <core/Operation.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <lookup/EntryLookup.hpp>

using namespace buddy::core;
using namespace buddy::lookup;

auto createOp(std::string&& data,
              std::string&& owner,
              std::size_t block,
              std::size_t value)
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
