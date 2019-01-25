#include <core/Operation.hpp>
#include <core/Transaction.hpp>
#include <gtest/gtest.h>

using namespace std::string_literals;
using namespace forge::core;

TEST(OperationTest, EntryCreationOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc75010101aabbccdddeadbeef").getValue();
    std::int64_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::int64_t value = 10;

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

    auto data = operationToMetadata(op);

    EXPECT_EQ(data, stringToByteVec("c6dc75010101aabbccdddeadbeef").getValue());
}

TEST(OperationTest, EntryRenewalOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc75010201aabbccdddeadbeef").getValue();
    std::int64_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::int64_t value = 10;

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

    auto data = operationToMetadata(op);

    EXPECT_EQ(data, stringToByteVec("c6dc75010201aabbccdddeadbeef").getValue());
}

TEST(OperationTest, OwnershipTransferOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc75010401aabbccdddeadbeef").getValue();
    std::int64_t block = 1000;
    auto old_owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    auto new_owner = "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W"s;
    std::int64_t value = 10;

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

    auto data = operationToMetadata(op);

    EXPECT_EQ(data, stringToByteVec("c6dc75010401aabbccdddeadbeef").getValue());
}

TEST(OperationTest, EntryUpdateOpParsingValid)
{
    auto metadata = extractMetadata("6a00c6dc75010801ffffffffdeadbeef").getValue();
    std::int64_t block = 1000;
    auto old_owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    auto new_owner = "oMaZKaWWyu6Zqrs5ck3DXgFbMEre7Jo58W"s;
    std::int64_t value = 10;

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

    auto data = operationToMetadata(op);

    EXPECT_EQ(data, stringToByteVec("c6dc75010801ffffffffdeadbeef").getValue());
}

TEST(OperationTest, CreationOpMetadataCreation)
{
    auto expected_metadata = extractMetadata("6a00c6dc75010101aabbccdddeadbeef").getValue();

    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    std::array value1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    std::pair expected1{key1, forge::core::EntryValue{value1}};

    auto created_metadata = createEntryCreationOpMetadata(std::move(expected1));

    EXPECT_EQ(created_metadata, expected_metadata);
}

TEST(OperationTest, RenewalOpMetadataCreation)
{
    auto expected_metadata = extractMetadata("6a00c6dc75010201aabbccdddeadbeef").getValue();

    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    std::array value1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    std::pair expected1{key1, forge::core::EntryValue{value1}};

    auto created_metadata = createEntryRenewalOpMetadata(std::move(expected1));

    EXPECT_EQ(created_metadata, expected_metadata);
}

TEST(OperationTest, OwnershipTransferOpMetadataCreation)
{
    auto expected_metadata = extractMetadata("6a00c6dc75010401aabbccdddeadbeef").getValue();

    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    std::array value1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    std::pair expected1{key1, forge::core::EntryValue{value1}};

    auto created_metadata = createOwnershipTransferOpMetadata(std::move(expected1));

    EXPECT_EQ(created_metadata, expected_metadata);
}

TEST(OperationTest, UpdateOpMetadataCreation)
{
    auto expected_metadata = extractMetadata("6a00c6dc75010801aabbccdddeadbeef").getValue();

    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    std::array value1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    auto created_metadata = createEntryUpdateOpMetadata(std::move(key1),
                                                        std::move(value1));

    EXPECT_EQ(created_metadata, expected_metadata);
}


TEST(OperationTest, DeletionOpMetadataCreation)
{
    auto expected_metadata = extractMetadata("6a00c6dc75011001aabbccdddeadbeef").getValue();

    std::vector key1{
        (std::byte)0xde,
        (std::byte)0xad,
        (std::byte)0xbe,
        (std::byte)0xef};

    std::array value1{
        (std::byte)0xaa,
        (std::byte)0xbb,
        (std::byte)0xcc,
        (std::byte)0xdd};

    std::pair expected1{key1, forge::core::EntryValue{value1}};

    auto created_metadata = createEntryDeletionOpMetadata(std::move(expected1));

    EXPECT_EQ(created_metadata, expected_metadata);
}
