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
}