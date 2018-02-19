#include "gtest/gtest.h"

TEST(foo, bar)
{
	EXPECT_EQ(10, 10);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
