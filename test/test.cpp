#include "gtest/gtest.h"
#define REALNET_TEST

#define TEST_FRIEND \
    FRIEND_TEST(Address, foo);

#include <Address.hpp>


namespace Net
{
    namespace Core
    {
    }

    TEST(Address, foo)
    {
        Net::Address a;
        a.m_Bytes[0] = 1;

        EXPECT_EQ(a.m_Bytes[0], 1);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
