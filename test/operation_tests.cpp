#include <core/Operation.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace buddy::core;

TEST(OperationTest, EntryCreationOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc750101aabbccdddeadbeef").getValue();
    std::size_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::size_t value = 10;

    auto op_opt = parseMetadata(std::move(metadata),
                                block,
                                std::move(owner),
                                value);

    ASSERT_TRUE(op_opt);

    const auto& op = op_opt.getValue();

    ASSERT_TRUE(std::holds_alternative<EntryCreationOp>(op));

    auto creation = std::get<EntryCreationOp>(op);

    EXPECT_EQ(creation.getOwner(), "oLupzckPUYtGydsBisL86zcwsBweJm1dSM");
    EXPECT_EQ(creation.getEntryKey(), stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(creation.getValue(), 10);
    EXPECT_EQ(creation.getBlock(), 1000);
}

TEST(OperationTest, EntryRenewalOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc750201aabbccdddeadbeef").getValue();
    std::size_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::size_t value = 10;

    auto op_opt = parseMetadata(std::move(metadata),
                                block,
                                std::move(owner),
                                value);

    ASSERT_TRUE(op_opt);

    const auto& op = op_opt.getValue();

    ASSERT_TRUE(std::holds_alternative<EntryRenewalOp>(op));

    auto creation = std::get<EntryRenewalOp>(op);

    EXPECT_EQ(creation.getOwner(), "oLupzckPUYtGydsBisL86zcwsBweJm1dSM");
    EXPECT_EQ(creation.getEntryKey(), stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(creation.getBlock(), 1000);
}

TEST(OperationTest, OwnershipTransferOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc750401aabbccdddeadbeef").getValue();
    std::size_t block = 1000;
    auto old_owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    auto new_owner = "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W"s;
    std::size_t value = 10;

    auto op_opt = parseMetadata(std::move(metadata),
                                block,
                                std::move(old_owner),
                                value,
                                std::move(new_owner));

    ASSERT_TRUE(op_opt);

    const auto& op = op_opt.getValue();

    ASSERT_TRUE(std::holds_alternative<OwnershipTransferOp>(op));

    auto creation = std::get<OwnershipTransferOp>(op);

    EXPECT_EQ(creation.getOwner(), "oLupzckPUYtGydsBisL86zcwsBweJm1dSM");
    EXPECT_EQ(creation.getNewOwner(), "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W");
    EXPECT_EQ(creation.getEntryKey(), stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(creation.getBlock(), 1000);
}

TEST(OperationTest, EntryUpdateOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc750801ffffffffdeadbeef").getValue();
    std::size_t block = 1000;
    auto old_owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    auto new_owner = "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W"s;
    std::size_t value = 10;

    auto op_opt = parseMetadata(std::move(metadata),
                                block,
                                std::move(old_owner),
                                value);

    ASSERT_TRUE(op_opt);

    const auto& op = op_opt.getValue();

    ASSERT_TRUE(std::holds_alternative<EntryUpdateOp>(op));

    auto creation = std::get<EntryUpdateOp>(op);

    std::array expected{(std::byte)0xff,
                        (std::byte)0xff,
                        (std::byte)0xff,
                        (std::byte)0xff};

    EXPECT_EQ(creation.getOwner(), "oLupzckPUYtGydsBisL86zcwsBweJm1dSM");
    EXPECT_EQ(creation.getEntryKey(), stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(creation.getNewEntryValue(), EntryValue{expected});
    EXPECT_EQ(creation.getBlock(), 1000);
}
