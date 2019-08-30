#include "entrys/token/UtilityTokenCreationOp.hpp"
#include <core/Transaction.hpp>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <gtest/gtest.h>
#include <variant>

using namespace forge::core;
using namespace std::string_literals;

TEST(UtilityTokenOperationTest, UtilityTokenCreationOpParsingValid)
{
    auto metadata = extractMetadata(
                        "6a00" //OP_RETURN mumbojumbo
                        "c6dc75" //forge identifier
                        "03" //token type
                        "01" //operation flag
                        "0000000000000011" // amount 3
                        "deadbeef") //identifier
                        .getValue();

    std::int64_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::int64_t burn_value = 10;

    auto op_opt = forge::core::parseMetadataToUtilityTokenOp(metadata,
                                                             block,
                                                             std::move(owner),
                                                             burn_value);

    ASSERT_TRUE(op_opt);

    const auto& op = op_opt.getValue();

    ASSERT_TRUE(std::holds_alternative<UtilityTokenCreationOp>(op));

    auto creation = std::get<UtilityTokenCreationOp>(op);

    EXPECT_EQ(creation.getCreator(), "oLupzckPUYtGydsBisL86zcwsBweJm1dSM");
    EXPECT_EQ(creation.getUtilityToken().getId(), stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(creation.getBurnValue(), 10);
    EXPECT_EQ(creation.getBlock(), 1000);

    auto data = createUtilityTokenCreationOpMetadata(std::move(creation.getUtilityToken()),
                                                     creation.getAmount());

    EXPECT_EQ(data, stringToByteVec("c6dc7503010000000000000011deadbeef").getValue());
}

TEST(UtilityTokenOperationTest, UtilityTokenCreationOpParsingInvalid)
{
    auto metadata = extractMetadata(
                        "6a00" //OP_RETURN mumbojumbo
                        "c6dc75" //forge identifier
                        "03" //token type
                        "05" //INVALID OP FLAG
                        "0000000000000011" // amount 3
                        "deadbeef") //identifier
                        .getValue();

    std::int64_t block = 1000;
    auto owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    std::int64_t burn_value = 10;

    auto op_opt = forge::core::parseMetadataToUtilityTokenOp(metadata,
                                                             block,
                                                             std::move(owner),
                                                             burn_value);

    ASSERT_FALSE(op_opt);

    metadata = extractMetadata(
                   "6a00" //OP_RETURN mumbojumbo
                   "c6dc75" //forge identifier
                   "03" //token type
                   "04" //WRONG OP FLAG
                   "0000000000000011" // amount 3
                   "deadbeef") //identifier
                   .getValue();

    owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    op_opt = forge::core::parseMetadataToUtilityTokenOp(metadata,
                                                        block,
                                                        std::move(owner),
                                                        burn_value);

    ASSERT_TRUE(op_opt);
    ASSERT_FALSE(std::holds_alternative<UtilityTokenCreationOp>(op_opt.getValue()));

    metadata = extractMetadata(
                   "6a00" //OP_RETURN mumbojumbo
                   "c6dc75" //forge id
                   "02" //WRONG TOKEN TYPE
                   "01" //op flag
                   "0000000000000011" // amount 3
                   "deadbeef") //identifier
                   .getValue();

    owner = "oLupzckPUYtGydsBisL86zcwsBweJm1dSM"s;
    op_opt = forge::core::parseMetadataToUtilityTokenOp(metadata,
                                                        block,
                                                        std::move(owner),
                                                        burn_value);

    ASSERT_FALSE(op_opt);
}
