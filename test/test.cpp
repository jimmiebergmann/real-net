#include "gtest/gtest.h"
#include <cstring>
#include <chrono>
#include <thread>

#define REALNET_TEST

#define TEST_FRIEND \
    FRIEND_TEST(Address, tests); \
    FRIEND_TEST(SocketAddress, tests);

#include <Clock.hpp>
#include <Time.hpp>
#include <Address.hpp>




namespace Net
{
    namespace Core
    {
    }

    TEST(Time, tests)
    {
        // Creation
        {
            const Net::Time time = Net::Seconds(1.0f);
            EXPECT_NE(time, Time::Zero);
            EXPECT_NE(time, Time::Infinite);
            EXPECT_EQ(time.AsSeconds(), 1.0f);
            EXPECT_EQ(time.AsMilliseconds(), 1000ULL);
            EXPECT_EQ(time.AsMicroseconds(), 1000000ULL);

        }
        {
            const Net::Time time = Net::Milliseconds(1000ULL);
            EXPECT_NE(time, Time::Zero);
            EXPECT_NE(time, Time::Infinite);
            EXPECT_EQ(time.AsSeconds(), 1.0f);
            EXPECT_EQ(time.AsMilliseconds(), 1000ULL);
            EXPECT_EQ(time.AsMicroseconds(), 1000000ULL);
        }
        {
            const Net::Time time = Net::Microseconds(1000000ULL);
            EXPECT_NE(time, Time::Zero);
            EXPECT_NE(time, Time::Infinite);
            EXPECT_EQ(time.AsSeconds(), 1.0f);
            EXPECT_EQ(time.AsMilliseconds(), 1000ULL);
            EXPECT_EQ(time.AsMicroseconds(), 1000000ULL);
        }
        // Operators
        {
            const Net::Time time_1 = Net::Microseconds(100ULL);
            const Net::Time time_2 = Net::Microseconds(200ULL);

            EXPECT_TRUE(time_1 == time_1);
            EXPECT_FALSE(time_1 == time_2);

            EXPECT_TRUE(time_1 != time_2);
            EXPECT_FALSE(time_1 != time_1);

            EXPECT_TRUE(time_2 > time_1);
            EXPECT_FALSE(time_1 > time_2);

            EXPECT_TRUE(time_1 < time_2);
            EXPECT_FALSE(time_2 < time_1);

            EXPECT_TRUE(time_2 >= time_1);
            EXPECT_TRUE(time_2 >= time_2);
            EXPECT_FALSE(time_1 >= time_2);

            EXPECT_TRUE(time_1 <= time_2);
            EXPECT_TRUE(time_1 <= time_1);
            EXPECT_FALSE(time_2 <= time_1);

            EXPECT_EQ(time_1 + time_1, time_2);
            EXPECT_NE(time_1 + time_2, time_2);

            EXPECT_EQ(time_2 - time_1, time_1);
            EXPECT_NE(time_2 - time_2, time_1);
            EXPECT_EQ(time_2 - time_2, Time::Zero);

            EXPECT_EQ(time_1 / 5ULL, Net::Microseconds(20ULL));
            EXPECT_NE(time_1 / 10ULL, Net::Microseconds(20ULL));

            EXPECT_EQ(time_1 % 55ULL, Net::Microseconds(45ULL));
            EXPECT_EQ(time_1 % 75ULL, Net::Microseconds(25ULL));

            {
                Net::Time time = Net::Microseconds(100ULL);
                time -= Net::Microseconds(65ULL);
                EXPECT_EQ(time, Net::Microseconds(35ULL));
            }
            {
                Net::Time time = Net::Microseconds(100ULL);
                time += Net::Microseconds(65ULL);
                EXPECT_EQ(time, Net::Microseconds(165ULL));
            }
        }

    }

    TEST(Clock, tests)
    {
        {
            Net::Clock clock;
            clock.Start();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            clock.Stop();
            EXPECT_TRUE(clock.GetLapsedTime() >= Net::Microseconds(100ULL));
        }

        {
            Net::Clock clock;
            clock.Start();
            Net::Time lastTime = Net::Time::Zero;
            for(size_t i = 0; i < 10; i++)
            {
               std::this_thread::sleep_for(std::chrono::microseconds(100));
               EXPECT_TRUE(clock.GetLapsedTime() >= lastTime);
               lastTime = clock.GetLapsedTime();
            }
        }
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

                address.SetLoopback();
                EXPECT_FALSE(address.IsZero());
                EXPECT_TRUE(address.IsLoopback());

                address.SetZero();
                EXPECT_TRUE(address.IsZero());
                EXPECT_FALSE(address.IsLoopback());
            }
        }

        // Constructor/hostname tests
        {
            {
                Net::Address address("localhost");
                EXPECT_EQ(address.GetType(), Net::Address::Ipv4);
                EXPECT_TRUE(address.IsLoopback());
                EXPECT_FALSE(address.IsZero());
                EXPECT_TRUE(address.GetAsString() == "127.0.0.1");
            }
            {
                Net::Address address("ip6-localhost");
                EXPECT_EQ(address.GetType(), Net::Address::Ipv6);
                EXPECT_TRUE(address.IsLoopback());
                EXPECT_FALSE(address.IsZero());
            }
            {
                Net::Address address;
                EXPECT_TRUE(address.Set("www.google.com"));
                EXPECT_FALSE(address.IsZero());
                EXPECT_FALSE(address.Set("thisisaninvalidaddress1234567890"));
                EXPECT_TRUE(address.IsZero());
            }
        }

        // Bytes test
        {
            {
                Net::Address address;
                EXPECT_TRUE(std::memcmp(address.GetBytes(), zeroArray16, 16) == 0);
            }
            {
                Net::Address address;
                address.SetLoopback(Net::Address::Ipv4);
                EXPECT_TRUE(std::memcmp(address.GetBytes(), ipv4LoopbackArray, 16) == 0);
            }
            {
                Net::Address address;
                address.SetLoopback(Net::Address::Ipv6);
                EXPECT_TRUE(std::memcmp(address.GetBytes(), ipv6LoopbackArray, 16) == 0);
            }
        }

        // Compare operators
        {
            EXPECT_TRUE(Net::Address::Zero == Net::Address::Zero);

            Net::Address address;
            address.SetLoopback(Net::Address::Ipv4);
            EXPECT_FALSE(address == Net::Address::Zero);
            EXPECT_TRUE(address == address);
            EXPECT_TRUE(address == Net::Address::Loopback(Net::Address::Ipv4));
        }
    }

    TEST(SocketAddress, tests)
    {
        Net::Address address;
        address.SetLoopback(Net::Address::Ipv4);

        Net::SocketAddress socketAddress(address, 123);

        EXPECT_EQ(socketAddress.GetIp().GetType(), Net::Address::Ipv4);
        EXPECT_TRUE(socketAddress.GetIp().IsLoopback());
        socketAddress.SetIp(Net::Address::Zero);
        EXPECT_TRUE(socketAddress.GetIp().IsZero());

        EXPECT_EQ(socketAddress.GetPort(), 123);
        socketAddress.SetPort(321);
        EXPECT_EQ(socketAddress.GetPort(), 321);
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
