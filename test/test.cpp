#include "gtest/gtest.h"
#include <cstring>

#define REALNET_TEST

#define TEST_FRIEND \
    FRIEND_TEST(Address, tests);

#include <Address.hpp>


namespace Net
{
    namespace Core
    {
    }

    TEST(Address, tests)
    {
        const unsigned char zeroArray16[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        const unsigned char ipv4LoopbackArray[16] = {127, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        const unsigned char ipv6LoopbackArray[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

        // Loopback/Zero tests
        {
            {
                Net::Address address;
                EXPECT_TRUE(address.IsZero());
                EXPECT_FALSE(address.IsLoopback());

                address.Loopback();
                EXPECT_FALSE(address.IsZero());
                EXPECT_TRUE(address.IsLoopback());

                address.Zero();
                EXPECT_TRUE(address.IsZero());
                EXPECT_FALSE(address.IsLoopback());
            }
        }

        // Constructor/hostname tests
        {
            {
                Net::Address address("localhost");
                EXPECT_EQ(address.Type(), Net::Address::Ipv4);
                EXPECT_TRUE(address.IsLoopback());
                EXPECT_FALSE(address.IsZero());
                EXPECT_TRUE(address.String() == "127.0.0.1");
            }
            {
                Net::Address address("ip6-localhost");
                EXPECT_EQ(address.Type(), Net::Address::Ipv6);
                EXPECT_TRUE(address.IsLoopback());
                EXPECT_FALSE(address.IsZero());
            }
            {
                Net::Address address;
                EXPECT_TRUE(address.Hostname("www.google.com"));
                EXPECT_FALSE(address.IsZero());
                EXPECT_FALSE(address.Hostname("thisisaninvalidaddress1234567890"));
                EXPECT_TRUE(address.IsZero());
            }
        }

        // Bytes test
        {
            {
                Net::Address address;
                EXPECT_TRUE(std::memcmp(address.Bytes(), zeroArray16, 16) == 0);
            }
            {
                Net::Address address;
                address.Loopback(Net::Address::Ipv4);
                EXPECT_TRUE(std::memcmp(address.Bytes(), ipv4LoopbackArray, 16) == 0);
            }
            {
                Net::Address address;
                address.Loopback(Net::Address::Ipv6);
                EXPECT_TRUE(std::memcmp(address.Bytes(), ipv6LoopbackArray, 16) == 0);
            }
        }
    }

    TEST(SocketAddress, tests)
    {
        Net::Address address;
        address.Loopback(Net::Address::Ipv4);

        Net::SocketAddress socketAddress(address, 123);

        EXPECT_EQ(socketAddress.AddressRef().Type(), Net::Address::Ipv4);
        EXPECT_TRUE(socketAddress.AddressRef().IsLoopback());
        socketAddress.AddressRef().Zero();
        EXPECT_TRUE(socketAddress.AddressRef().IsZero());

        EXPECT_EQ(socketAddress.PortRef(), 123);
        socketAddress.PortRef() = 321;
        EXPECT_EQ(socketAddress.PortRef(), 321);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
