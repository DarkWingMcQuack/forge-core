#include "utilxx/Opt.hpp"
#include <core/Transaction.hpp>
#include <cstdint>
#include <entrys/token/UtilityTokenOperation.hpp>
#include <gtest/gtest.h>
#include <lookup/UtilityTokenLookup.hpp>
#include <variant>

using namespace forge::core;
using forge::lookup::UtilityTokenLookup;
using namespace std::string_literals;

namespace {
auto createOp(const std::string& op,
              std::int64_t block,
              std::string owner,
              std::int64_t burn_value,
              utilxx::Opt<std::string> new_owner = std::nullopt)
{
    auto metadata = stringToByteVec(op).getValue();

    return parseMetadataToUtilityTokenOp(metadata,
                                         block,
                                         std::move(owner),
                                         burn_value,
                                         std::move(new_owner))
        .getValue();
}
} // namespace

TEST(UtilityTokenLookupTest, CreationOpExecutionTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    lookup.executeOperations({creation_op1});

    auto available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                  stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available,
              3);
}

TEST(UtilityTokenLookupTest, CreationOpCompetitorExecutionTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    auto creation_op2 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
        11);

    lookup.executeOperations({creation_op1,
                              creation_op2});

    auto available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                   stringToByteVec("deadbeef").getValue());

    auto available2 = lookup.getAvailableBalanceOf("oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
                                                   stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available1,
              0);

    EXPECT_EQ(available2,
              3);


    auto creation_op3 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        101,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        110);

    lookup.executeOperations({creation_op3});

    auto available3 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                   stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available3,
              0);
}

TEST(UtilityTokenLookupTest, TransferOpExecutionTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);


    auto transfer_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "02" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10,
        "oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z"s);

    auto transfer_op2 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "02" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
        10,
        "oP9UPtBMngCMNfr7pDPA3vBnfMQEWFL1cP"s);

    lookup.executeOperations({creation_op1});

    auto available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              3);

    lookup.executeOperations({transfer_op1});

    available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                              stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              0);

    auto available2 = lookup.getAvailableBalanceOf("oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available2,
              3);

    lookup.executeOperations({transfer_op2});

    available2 = lookup.getAvailableBalanceOf("oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
                                              stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available2,
              0);

    auto available3 = lookup.getAvailableBalanceOf("oP9UPtBMngCMNfr7pDPA3vBnfMQEWFL1cP",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available3,
              3);
}

TEST(UtilityTokenLookupTest, TransferOverpayTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);


    auto transfer_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "02" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10,
        "oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z"s);

    auto transfer_op2 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "02" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        11,
        "oP9UPtBMngCMNfr7pDPA3vBnfMQEWFL1cP"s);

    lookup.executeOperations({creation_op1});

    auto available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              3);

    lookup.executeOperations({transfer_op1, transfer_op2});

    available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                              stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              0);

    auto available2 = lookup.getAvailableBalanceOf("oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available2,
              0);

    auto available3 = lookup.getAvailableBalanceOf("oP9UPtBMngCMNfr7pDPA3vBnfMQEWFL1cP",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available3,
              3);
}

TEST(UtilityTokenLookupTest, DeletionOpExecutionTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    auto deletion_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "04" //operation flag
        "0000000000000002" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    lookup.executeOperations({creation_op1});

    auto available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                   stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              3);

    lookup.executeOperations({deletion_op1});

    available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                              stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              1);


    //TRY TO DELETE MORE THAN AVAILABLE
    lookup.executeOperations({deletion_op1});

    available1 = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                              stringToByteVec("deadbeef").getValue());
    EXPECT_EQ(available1,
              1);
}


TEST(UtilityTokenLookupTest, DoubleCreationTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    auto creation_op2 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "0000000000000003" // amount 3
        "deadbeef",
        100,
        "oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
        11);

    lookup.executeOperations({creation_op1});

    auto available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                  stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available,
              3);

    lookup.executeOperations({creation_op1});

    available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                             stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available,
              3);

    lookup.executeOperations({creation_op2});

    available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                             stringToByteVec("deadbeef").getValue());

    auto available2 = lookup.getAvailableBalanceOf("oHe5FSnZxgs81dyiot1FuSJNuc1mYWYd1Z",
                                                   stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available2,
              0);

    EXPECT_EQ(available,
              3);
}

TEST(UtilityTokenLookupTest, OverflowTest)
{
    UtilityTokenLookup lookup;

    auto creation_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "01" //operation flag
        "FF00000000000000" // amount
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    auto deletion_op1 = createOp(
        "c6dc75" //forge identifier
        "03" //token type
        "04" //operation flag
        "FF00000000000000" // amount 3
        "deadbeef",
        100,
        "oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
        10);

    lookup.executeOperations({creation_op1});

    auto available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                                  stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available,
              0xFF00000000000000);

    //the second operation would overflow the
    //total amout of used tokens
    //thats why only the first operation should be executed
    lookup.executeOperations({deletion_op1,
                              deletion_op1});

    available = lookup.getAvailableBalanceOf("oLupzckPUYtGydsBisL86zcwsBweJm1dSM",
                                             stringToByteVec("deadbeef").getValue());

    EXPECT_EQ(available,
              0);
}
