/*
* MIT License
*
* Copyright(c) 2018 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include <core/UdpSocket.hpp>
#include <cstring>

namespace Net
{

    namespace Core
    {

        UdpSocket::UdpSocket() :
            m_Handle(0),
            m_Family(Address::Any)
        {
        }

        UdpSocket::UdpSocket(const unsigned short port, Address::eType type) :
            m_Handle(0)
        {
            Open(port, type);
        }

        UdpSocket::~UdpSocket()
        {
            Close();
        }

        void UdpSocket::Open(const unsigned short port, const Address::eType family)
        {
            Close();

            int domain = (family == Address::Ipv4 ? AF_INET : AF_INET6);

            // Create the socket
            if((m_Handle = socket(domain, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
            {
                throw SystemException("Failed to create socket.", GetLastSystemError());
            }

            // Ipv6/Any
            if(family == Address::Any || family == Address::Ipv6)
            {
                const int v6only = family == Address::Any ? 0 : 1;

                // Enable/disable IPV6_V6ONLY
                if (setsockopt(m_Handle, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only)) != 0)
                {
                    Close();
                    throw SystemException("Failed to enable/disable ipv6 only.", GetLastSystemError());
                }

                struct sockaddr_in6 service;
                memset( &service, 0, sizeof(service));
                service.sin6_family = AF_INET6;
                service.sin6_addr = in6addr_any;
                service.sin6_port = htons(port);

                // Bind the socket
                if(bind(m_Handle, reinterpret_cast<const sockaddr *>(&service), sizeof(service)) != 0)
                {
                    Close();
                    throw SystemException("Failed to bind socket to port " + std::to_string(port) +  ".", GetLastSystemError());
                }

                m_Family = Address::Ipv6;
                m_SocketAddress.SetIp(Address::Loopback(m_Family));
            }
            // Ipv4
            else
            {
                struct sockaddr_in service;
                memset( &service, 0, sizeof(service));
                service.sin_family = domain;
                service.sin_addr.s_addr = htonl(INADDR_ANY);
                service.sin_port = htons(port);

                // Bind the socket
                if(bind(m_Handle, reinterpret_cast<const sockaddr *>(&service), sizeof(service)) != 0)
                {
                    Close();
                    throw SystemException("Failed to bind socket to port " + std::to_string(port) +  ".", GetLastSystemError());
                }

                m_Family = Address::Ipv6;
                m_SocketAddress.SetIp(Address::Loopback(m_Family));
            }

            m_SocketAddress.SetPort(port);
        }

        void UdpSocket::Close()
        {
            if(m_Handle != 0)
            {
                closesocket(m_Handle);
                m_Handle = 0;
                m_Family = Address::Any;
            }
        }

        int UdpSocket::Send(const void * data, const size_t size, const SocketAddress & socketAddress)
        {
            const Address & address = socketAddress.GetIp();
            const unsigned short port = socketAddress.GetPort();
            int result = 0;

            if(address.GetType() == Net::Address::Ipv4)
            {
                sockaddr_in outAddr;
                memset (&outAddr, 0, sizeof(outAddr));
                outAddr.sin_family = AF_INET;
                outAddr.sin_port = htons(port);
                const unsigned char * pBytes = address.GetBytes();
                const unsigned int ipv4 = (static_cast<unsigned int>(pBytes[0]) << 24) |
                                          (static_cast<unsigned int>(pBytes[1]) << 16) |
                                          (static_cast<unsigned int>(pBytes[2]) << 8)  |
                                           static_cast<unsigned int>(pBytes[3]);
                outAddr.sin_addr.s_addr = htonl(ipv4);

                result = sendto(m_Handle, data, size, 0, (struct sockaddr*)&outAddr, sizeof(outAddr));
            }
            else if(address.GetType() == Net::Address::Ipv6)
            {
                struct sockaddr_in6 outAddr;
                memset (&outAddr, 0, sizeof(outAddr));
                outAddr.sin6_family = AF_INET6;
                outAddr.sin6_port = htons(port);
                memcpy(outAddr.sin6_addr.s6_addr, address.GetBytes(), 16);

                result = sendto(m_Handle, data, size, 0, (struct sockaddr*)&outAddr, sizeof(outAddr));
            }

            return result;
        }

        int UdpSocket::Receive(void * data, const size_t size, SocketAddress & socketAddress)
        {
            struct sockaddr_storage from;
            socklen_t fromLength = sizeof(from);
            int result = recvfrom(m_Handle, data, size, 0, (struct sockaddr*)&from, &fromLength);
            if(result < 0)
            {
                return result;
            }

            // Get sender address.
            switch (from.ss_family)
            {
                case AF_INET:
                {
                    struct sockaddr_in * pIp = (struct sockaddr_in*)&from;
                    const unsigned int ipv4 = ntohl(static_cast<unsigned int>(pIp->sin_addr.s_addr));

                    Address address(static_cast<unsigned char>(ipv4 >> 24),
                                    static_cast<unsigned char>(ipv4 >> 16),
                                    static_cast<unsigned char>(ipv4 >> 8),
                                    static_cast<unsigned char>(ipv4));
                    socketAddress.SetIp(address);
                    socketAddress.SetPort(ntohs(pIp->sin_port));
                }
                break;
                case AF_INET6:
                {
                    struct sockaddr_in6 * pIp = (struct sockaddr_in6*)&from;
                    const unsigned char * ipv6 = static_cast<unsigned char *>(pIp->sin6_addr.s6_addr);
                    Address address(ipv6);
                    socketAddress.SetIp(address);
                    socketAddress.SetPort(ntohs(pIp->sin6_port));
                }
                break;
                default:
                    break;
            }

            return result;
        }

        void UdpSocket::SetBlocking(const bool status)
        {
        #if defined(REALNET_PLATFORM_WINDOWS)
            unsigned long mode = blocking ? 0 : 1;
            if(ioctlsocket(m_Handle, FIONBIO, &mode) != 0)
            {
                throw SystemException("Failed set socket blocking.", GetLastSystemError());
            }
        #elif defined(REALNET_PLATFORM_LINUX)
            int flags = fcntl(m_Handle, F_GETFL, 0);
            if (flags == -1)
            {
               throw SystemException("Failed get socket blocking.", GetLastSystemError());
            }
            flags = status ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
            if(fcntl(m_Handle, F_SETFL, flags) != 0)
            {
                throw SystemException("Failed set socket blocking.", GetLastSystemError());
            }
        #endif
        }

        Address::eType UdpSocket::GetFamily() const
        {
            return m_Family;
        }

        SocketHandle UdpSocket::GetHandle() const
        {
            return m_Handle;
        }

        SocketAddress UdpSocket::GetSocketAddress() const
        {
            return m_SocketAddress;
        }

    }

}
