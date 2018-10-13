#include "resource_path.hpp"
#include <gtest/gtest.h>

auto main(int argc, char **argv)
    -> int
{
    ::testing::InitGoogleTest(&argc, argv);

    resource_path = argv[1];

    return RUN_ALL_TESTS();
}
