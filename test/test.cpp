#include "gtest/gtest.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>
#include <list>

#define REALNET_TEST

#define REALNET_TEST_FRIEND \
    FRIEND_TEST(EntityManager, EntityLinkage);


#include <Clock.hpp>
#include <Time.hpp>
#include <Address.hpp>
#include <Server.hpp>
#include <core/UdpSocket.hpp>
#include <core/SocketSelector.hpp>
#include <core/Md5.hpp>
#include <core/PacketPool.hpp>
#include <core/Latency.hpp>


#define GTEST_PRINT "\033[0;32m" << "[          ] " << "\033[0;0m"

namespace Net
{

    namespace Core
    {
    }

   /* TEST(Md5, tests)
    {
        {
            Net::Core::Md5 md5;

            md5.Hash("The quick brown fox jumps over the lazy dog");
            EXPECT_EQ(md5.GetAsString(), "9e107d9d372bb6826bd81d3542a419d6");
        }
    }*/

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

            // Equal
            EXPECT_TRUE(time_1 == time_1);
            EXPECT_FALSE(time_1 == time_2);

            // Not equal
            EXPECT_TRUE(time_1 != time_2);
            EXPECT_FALSE(time_1 != time_1);

            // Larger
            EXPECT_TRUE(time_2 > time_1);
            EXPECT_FALSE(time_1 > time_2);

            // Less
            EXPECT_TRUE(time_1 < time_2);
            EXPECT_FALSE(time_2 < time_1);

            // Larger equal
            EXPECT_TRUE(time_2 >= time_1);
            EXPECT_TRUE(time_2 >= time_2);
            EXPECT_FALSE(time_1 >= time_2);

            // Less equal
            EXPECT_TRUE(time_1 <= time_2);
            EXPECT_TRUE(time_1 <= time_1);
            EXPECT_FALSE(time_2 <= time_1);

            // Addition
            EXPECT_EQ(time_1 + time_1, time_2);
            EXPECT_NE(time_1 + time_2, time_2);
            EXPECT_EQ(time_1 + Net::Time::Infinite, Net::Time::Infinite);

            // Subtraction
            EXPECT_EQ(time_2 - time_1, time_1);
            EXPECT_NE(time_2 - time_2, time_1);
            EXPECT_EQ(time_2 - time_2, Time::Zero);

            // Division
            EXPECT_EQ(time_1 / 5ULL, Net::Microseconds(20ULL));
            EXPECT_EQ(time_1 / 5.0f, Net::Microseconds(20ULL));
            EXPECT_EQ(time_1 / 2.5f, Net::Microseconds(40ULL));
            EXPECT_NE(time_1 / 10ULL, Net::Microseconds(20ULL));
            EXPECT_NE(time_1 / 10.0f, Net::Microseconds(20ULL));

            EXPECT_EQ(time_1 / 100.0f, Net::Microseconds(1ULL));
            EXPECT_EQ(time_1 / 100.1f, Net::Microseconds(0ULL));
            EXPECT_NE(Net::Time::Infinite / 2ULL, Net::Time::Infinite);
            EXPECT_NE(Net::Time::Infinite / 2ULL, Net::Time::Zero);
            EXPECT_NE(Net::Time::Infinite / 1.25f, Net::Time::Infinite);
            EXPECT_NE(Net::Time::Infinite / 1.25f, Net::Time::Zero);


            // Multiplication
            EXPECT_EQ(time_1 * 5ULL, Net::Microseconds(500ULL));
            EXPECT_EQ(time_1 * 5.0f, Net::Microseconds(500ULL));
            EXPECT_EQ((Net::Time::Infinite / 1.25f) * 10ULL, Net::Time::Infinite);
            EXPECT_EQ((Net::Time::Infinite / 1.25f) * 10.0f, Net::Time::Infinite);


            // Modulus
            EXPECT_EQ(time_1 % 55ULL, Net::Microseconds(45ULL));
            EXPECT_EQ(time_1 % 75ULL, Net::Microseconds(25ULL));

            // Self subtraction
            {
                Net::Time time = Net::Microseconds(100ULL);
                time -= Net::Microseconds(65ULL);
                EXPECT_EQ(time, Net::Microseconds(35ULL));

                time = Net::Microseconds(100ULL);
                time -= Net::Microseconds(200ULL);
                EXPECT_EQ(time, Net::Time::Zero);
            }

            // Self addition
            {
                Net::Time time = Net::Microseconds(100ULL);
                time += Net::Microseconds(65ULL);
                EXPECT_EQ(time, Net::Microseconds(165ULL));

                time = Net::Time::Infinite;
                time += Net::Microseconds(100ULL);
                EXPECT_EQ(time, Net::Time::Infinite);

                time = Net::Microseconds(100ULL);
                time += Net::Time::Infinite;
                EXPECT_EQ(time, Net::Time::Infinite);
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
            EXPECT_TRUE(clock.LapsedTime() >= Net::Microseconds(100ULL));
        }

        {
            Net::Clock clock;
            clock.Start();
            Net::Time lastTime = Net::Time::Zero;
            for(size_t i = 0; i < 10; i++)
            {
               std::this_thread::sleep_for(std::chrono::microseconds(100));
               EXPECT_TRUE(clock.LapsedTime() >= lastTime);
               lastTime = clock.LapsedTime();
            }
        }
    }

    TEST(PacketPool, tests)
    {
        {
            Net::Core::PacketPool pool(1000);

            EXPECT_EQ(pool.GetPoolSize(),           1000);
            EXPECT_EQ(pool.DecreasePoolSize(50),    950);
            EXPECT_EQ(pool.DecreasePoolSize(50),    900);
            EXPECT_EQ(pool.GetTotalCount(),         1000);
            EXPECT_EQ(pool.GetFreeCount(),          1000);
            EXPECT_EQ(pool.GetPoolSize(),           900);
            EXPECT_EQ(pool.IncreasePoolSize(50),    950);
            EXPECT_EQ(pool.IncreasePoolSize(50),    1000);
            EXPECT_EQ(pool.GetPoolSize(),           1000);

            EXPECT_EQ(pool.DecreasePoolSize(2000),  0);
            EXPECT_EQ(pool.GetPoolSize(),           0);

            EXPECT_EQ(pool.IncreasePoolSize(1000),  1000);
            EXPECT_EQ(pool.GetPoolSize(),           1000);
        }
        {
            Net::Core::PacketPool pool(1000);

            std::list<Net::Core::Packet *> packets;

            for(size_t i = 0; i < 1000; i++)
            {
                EXPECT_EQ(pool.GetFreeCount(), 1000 - static_cast<int>(i));

                Net::Core::Packet * pPacket = pool.Get();
                EXPECT_NE(pPacket, nullptr);
                packets.push_back(pPacket);
            }
            for(auto it = packets.begin(); it != packets.end(); it++)
            {
                EXPECT_NO_THROW(pool.Return(*it));
            }
        }
        {
            Net::Core::PacketPool pool(1000);
            for(size_t i = 0; i < 2000; i++)
            {
                EXPECT_EQ(pool.GetFreeCount(), std::max<int>(1000 - static_cast<int>(i), 0));
                EXPECT_NE(pool.Get(), nullptr);
            }
        }
    }

    TEST(Latency, sample_count)
    {
        Net::Core::Latency latency_1(0);
        EXPECT_EQ(latency_1.GetSampleCount(), 1);

        Net::Core::Latency latency_2(1);
        EXPECT_EQ(latency_2.GetSampleCount(), 1);

        Net::Core::Latency latency_3(2);
        EXPECT_EQ(latency_3.GetSampleCount(), 2);
    }

    TEST(Latency, count)
    {
        Net::Core::Latency latency(3);
        EXPECT_EQ(latency.GetSampleCount(), 3);

        EXPECT_EQ(latency.GetCount(), 0);
        latency.Add(Net::Seconds(1.0f));
        EXPECT_EQ(latency.GetCount(), 1);
        latency.Add(Net::Seconds(2.0f));
        EXPECT_EQ(latency.GetCount(), 2);
        latency.Add(Net::Seconds(3.0f));
        EXPECT_EQ(latency.GetCount(), 3);
        latency.Add(Net::Seconds(4.0f));
        EXPECT_EQ(latency.GetCount(), 3);
    }

    TEST(Latency, median)
    {
        // 3 items
        {
            Net::Core::Latency latency(3);
            Net::Time time;

            latency.Add(Net::Microseconds(2));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 2);

            latency.Add(Net::Microseconds(4));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 3);

            latency.Add(Net::Microseconds(6));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 4);

            latency.Add(Net::Microseconds(7));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 6);

            latency.Add(Net::Microseconds(1));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 6);

            latency.Add(Net::Microseconds(10));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 7);

            latency.Add(Net::Microseconds(5));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 5);
        }

        // 4 items
        {
            Net::Core::Latency latency(4);
            Net::Time time;

            latency.Add(Net::Microseconds(2));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 2);

            latency.Add(Net::Microseconds(4));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 3);

            latency.Add(Net::Microseconds(6));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 4);

            latency.Add(Net::Microseconds(7));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 5);

            latency.Add(Net::Microseconds(10));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 6);

            latency.Add(Net::Microseconds(5));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 6);

            latency.Add(Net::Microseconds(10));
            latency.Get(time);
            EXPECT_EQ(time.AsMicroseconds(), 8);

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
                EXPECT_TRUE(address.AsString() == "127.0.0.1");
            }
            {
                Net::Address address("::1");
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

            EXPECT_TRUE(address != Net::Address::Loopback(Net::Address::Ipv6));
            EXPECT_FALSE(address != address);

            EXPECT_TRUE(address < Net::Address::Loopback(Net::Address::Ipv6));
            EXPECT_FALSE(Net::Address::Loopback(Net::Address::Ipv6) < address);

            EXPECT_FALSE(Net::Address(1, 2, 3, 4) < Net::Address(1, 2, 3, 4));
            EXPECT_TRUE(Net::Address(1, 2, 3, 4) < Net::Address(1, 2, 3, 5));
            EXPECT_FALSE(Net::Address(1, 2, 3, 5) < Net::Address(1, 2, 3, 4));
        }
    }

    TEST(SocketAddress, tests)
    {
        Net::Address address;
        address.SetLoopback(Net::Address::Ipv4);

        Net::SocketAddress socketAddress(address, 123);

        EXPECT_EQ(socketAddress.Ip.GetType(), Net::Address::Ipv4);
        EXPECT_TRUE(socketAddress.Ip.IsLoopback());
        socketAddress.Ip = Net::Address::Zero;
        EXPECT_TRUE(socketAddress.Ip.IsZero());

        EXPECT_EQ(socketAddress.Port, 123);
        socketAddress.Port = 321;
        EXPECT_EQ(socketAddress.Port, 321);

        EXPECT_TRUE(Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12345) <
                    Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12346));


        EXPECT_FALSE(Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12346) <
                    Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12345));

        EXPECT_FALSE(Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12345) <
                    Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12345));

    }

    TEST(UdpSocket_SocketSelector, tests)
    {
        {
           Net::Core::UdpSocket socket;
           EXPECT_EQ(socket.GetHandle(), 0);
           EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv4));
           EXPECT_NE(socket.GetHandle(), 0);
        }
        {
           Net::Core::UdpSocket socket;
           EXPECT_EQ(socket.GetHandle(), 0);
           EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv6));
           EXPECT_NE(socket.GetHandle(), 0);
        }
        {
           Net::Core::UdpSocket socket;
           EXPECT_EQ(socket.GetHandle(), 0);
           EXPECT_NO_THROW(socket.Open(12312, Net::Address::Any));
           EXPECT_NE(socket.GetHandle(), 0);
        }
        {
            Net::Core::UdpSocket socket_1;
            Net::Core::UdpSocket socket_2;

            EXPECT_NO_THROW(socket_1.Open(12312, Net::Address::Ipv4));
            EXPECT_THROW(socket_2.Open(12312, Net::Address::Ipv4), Net::SystemException);
#if defined(REALNET_PLATFORM_LINUX)
            EXPECT_EQ(Net::Core::GetLastSystemError(), EADDRINUSE);
            errno = 0;
#endif // defined
        }
        {
            Net::Core::UdpSocket socket;
            Net::Core::SocketSelector selector(&socket);
            EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv6));
            EXPECT_FALSE(selector.Select(Net::Seconds(1.0f)));
        }
        {
            const size_t sendSize = 13;
            const char sendData[sendSize] = "Hello world!";
            {
                Net::Core::UdpSocket socket;
                Net::Core::SocketSelector selector(&socket);
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv4));
                EXPECT_EQ((int)socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12312)), (int)sendSize );
                EXPECT_TRUE(selector.Select(Net::Seconds(1.0f)));
            }
            {
                Net::Core::UdpSocket socket;
                Net::Core::SocketSelector selector(&socket);
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv6));
                EXPECT_EQ((int)socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv6), 12312)), (int)sendSize );
                std::cout << GTEST_PRINT << "Expected last error: " << Net::Core::GetLastSystemError() << std::endl;
                EXPECT_TRUE(selector.Select(Net::Seconds(1.0f)));
            }
            {
                Net::Core::UdpSocket socket;
                Net::Core::SocketSelector selector(&socket);
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Any));
                Net::Core::UdpSocket sendSocket;
                EXPECT_NO_THROW(sendSocket.Open(12313, Net::Address::Ipv4));

                EXPECT_EQ((int)sendSocket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12312)), (int)sendSize );
                EXPECT_TRUE(selector.Select(Net::Seconds(1.0f)));
            }
            {
                Net::Core::UdpSocket socket;
                Net::Core::SocketSelector selector(&socket);
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Any));
                EXPECT_EQ((int)socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12312)), (int)sendSize );
                EXPECT_TRUE(selector.Select(Net::Seconds(1.0f)));
            }
            {
                Net::Core::UdpSocket socket;
                Net::Core::SocketSelector selector(&socket);
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Any));

                std::thread closeThread = std::thread([&selector]()
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    selector.Stop();
                });

                EXPECT_FALSE(selector.Select());
                closeThread.join();
            }
        }
        {
            Net::Core::UdpSocket socket;
            EXPECT_NO_THROW(socket.Open(12312));
            EXPECT_NO_THROW(socket.SetBlocking(false));

            Net::SocketAddress socketAddress;
            char data[32];
            EXPECT_EQ(socket.Receive(data, 32, socketAddress), -1);
        }
        {
            {
                Net::Core::UdpSocket socket;
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv4));

                const size_t sendSize = 13;
                const char sendData[sendSize] = "Hello world!";
                int sentSize = 0;
                EXPECT_EQ(sentSize = socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12312)), (int)sendSize );

                if(sentSize >= 0)
                {
                    Net::SocketAddress socketAddress;
                    char data[32];
                    EXPECT_EQ(socket.Receive(data, 32, socketAddress), sendSize);
                    EXPECT_STREQ(data, "Hello world!");
                    EXPECT_EQ(socketAddress.Ip.GetType(), Net::Address::Ipv4);
                    std::cout << GTEST_PRINT << "Ip: " << socketAddress.Ip.AsString() << std::endl;
                }
            }
            {
                Net::Core::UdpSocket socket;
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Ipv6));

                const size_t sendSize = 13;
                const char sendData[sendSize] = "Hello world!";
                int sentSize = 0;
                EXPECT_EQ(sentSize = socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv6), 12312)), (int)sendSize );

                if(sentSize >= 0)
                {
                    Net::SocketAddress socketAddress;
                    char data[32];
                    EXPECT_EQ(socket.Receive(data, 32, socketAddress), sendSize);
                    EXPECT_STREQ(data, "Hello world!");
                    EXPECT_EQ(socketAddress.Ip.GetType(), Net::Address::Ipv6);
                    std::cout << GTEST_PRINT << "Ip: " << socketAddress.Ip.AsString() << std::endl;
                }
            }
            {
                Net::Core::UdpSocket socket;
                EXPECT_NO_THROW(socket.Open(12312, Net::Address::Any));

                const int sendSize = 13;
                const char sendData[sendSize] = "Hello world!";
                {
                    int sentSize = 0;
                    EXPECT_EQ(sentSize = socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv4), 12312)), sendSize );
                    if(sentSize >= 0)
                    {
                        Net::SocketAddress socketAddress;
                        char data[32];
                        EXPECT_EQ(socket.Receive(data, 32, socketAddress), sendSize);
                        EXPECT_STREQ(data, "Hello world!");
                        EXPECT_EQ(socketAddress.Ip.GetType(), Net::Address::Ipv6);
                        std::cout << GTEST_PRINT << "Ip: " << socketAddress.Ip.AsString() << std::endl;
                    }
                }
                {
                    int sentSize = 0;
                    EXPECT_EQ(sentSize = socket.Send(sendData, sendSize, Net::SocketAddress(Net::Address::Loopback(Net::Address::Ipv6), 12312)), sendSize );
                    if(sentSize >= 0)
                    {
                        Net::SocketAddress socketAddress;
                        char data[32];
                        EXPECT_EQ(socket.Receive(data, 32, socketAddress), sendSize);
                        EXPECT_STREQ(data, "Hello world!");
                        EXPECT_EQ(socketAddress.Ip.GetType(), Net::Address::Ipv6);
                        std::cout << GTEST_PRINT << "Ip: " << socketAddress.Ip.AsString() << std::endl;
                    }
                }

            }
        }
    }

    TEST(Server, basic_tests)
    {
        {
            Net::Server server;
            EXPECT_NO_THROW(server.Host(12312, 32));
            EXPECT_THROW(server.Host(12312, 32), Net::Exception);
        }
        {
            Net::Server server_1;
            Net::Server server_2;
            EXPECT_NO_THROW(server_1.Host(12312, 32));
            EXPECT_THROW(server_2.Host(12312, 32), Net::SystemException);
        }
        {
            Net::Server server;
            EXPECT_NO_THROW(server.Host(12313, 32));
            std::this_thread::sleep_for(std::chrono::seconds(1));
            server.Stop();
        }
    }
    TEST(EntityManager, EntityLinkage)
    {
        class FirstEntity : public Net::Entity
        {

            public:

                Net::Variable<int>      Integer;
                Net::Variable<float>    Float;
                Net::Variable<double>   Double;

        };

        class SecondEntity : public Net::Entity
        {

            public:

                Net::Variable<int>      Integer;
                Net::Variable<float>    Float;
                Net::Variable<double>   Double;

        };

        {
            Net::EntityManager em;
            EXPECT_NO_THROW(em.LinkEntity<FirstEntity>("FirstEntity"));
            EXPECT_NO_THROW(em.LinkEntity<FirstEntity>("FirstEntity"));
            EXPECT_THROW(em.LinkEntity<SecondEntity>("FirstEntity"), Net::Exception);
            EXPECT_TRUE(em.IsLinked("FirstEntity"));
            EXPECT_FALSE(em.IsLinked("Unknown"));
        }
        {
            Net::EntityManager em;
            EXPECT_NO_THROW(em.LinkEntity<FirstEntity>("FirstEntity"));
            EXPECT_NO_THROW(em.LinkEntity<FirstEntity>("FirstEntity").
                                LinkVariable("Integer", &FirstEntity::Integer).
                                LinkVariable("Float", &FirstEntity::Float).
                                LinkVariable("Double", &FirstEntity::Double));

            EXPECT_TRUE(em.IsLinked("FirstEntity"));
            EXPECT_FALSE(em.IsLinked("Unknown"));
        }
        {
            Net::EntityManager em;
            EXPECT_NO_THROW(em.LinkEntity<FirstEntity>("FirstEntity"));
            auto & entityLink = em.LinkEntity<FirstEntity>("FirstEntity");

            entityLink.LinkVariable("Integer", &FirstEntity::Integer);
            entityLink.LinkVariable("Float", &FirstEntity::Float);
            entityLink.LinkVariable("Double", &FirstEntity::Double);

            EXPECT_TRUE(em.IsLinked("FirstEntity"));
            EXPECT_FALSE(em.IsLinked("Unknown"));

            EXPECT_TRUE(entityLink.IsLinked("Integer"));
            EXPECT_TRUE(entityLink.IsLinked("Float"));
            EXPECT_TRUE(entityLink.IsLinked("Double"));
            EXPECT_FALSE(entityLink.IsLinked("Unknown"));
        }
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
