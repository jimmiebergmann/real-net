#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

namespace Tests
{
    class Address;
}

#define TEST_FRIEND \
    friend class Tests::Address; \

#include <Address.hpp>

namespace Tests
{
    class Address
    {
    public:

        Address()
        {
            Net::Address address;
            address.m_Bytes[0] = 120;
        }

    };
}





TEST(Address, 1)
{
    Tests::Address test;

	//EXPECT_EQ(10, 10);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
