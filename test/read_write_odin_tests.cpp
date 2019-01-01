#include "parsing.hpp"
#include <core/Transaction.hpp>
#include <daemon/odin/ReadWriteOdinDaemon.hpp>
#include <gtest/gtest.h>
#include <json/value.h>

TEST(ReadWriteOdinDaemonTest, processGetBlockHashValid)
{
    using namespace std::string_literals;
    using buddy::core::stringToByteVec;

    auto response1 = "6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635"s;
    auto res_1 = buddy::daemon::odin::processGenerateRawTxResponse(response1);
    ASSERT_TRUE(res_1.hasValue());

    auto expected1 =
        stringToByteVec("6a14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635")
            .getValue();
    EXPECT_EQ(res_1.getValue(), expected1);


    auto response2 = "e1f0d6373e51c572df26f19e161bc0463978604c1148fb4d66fe7800e8c66f31"s;
    auto res_2 = buddy::daemon::odin::processGenerateRawTxResponse(response2);
    ASSERT_TRUE(res_2.hasValue());

    auto expected2 =
        stringToByteVec("e1f0d6373e51c572df26f19e161bc0463978604c1148fb4d66fe7800e8c66f31")
            .getValue();
    EXPECT_EQ(res_2.getValue(), expected2);
}

TEST(ReadWriteOdinDaemonTest, processGetBlockHashInvalid)
{
    using namespace std::string_literals;
    using buddy::core::stringToByteVec;

    auto response1 = "6X14e96a93444c92a7db9accfc4e4675af6ea4c2a74676a025109df614640635"s;
    auto res_1 = buddy::daemon::odin::processGenerateRawTxResponse(response1);
    ASSERT_TRUE(res_1.hasError());

    auto res_2 = buddy::daemon::odin::processGenerateRawTxResponse(10);
    ASSERT_TRUE(res_2.hasError());
}
