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
            m_Handle(0)
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

        void UdpSocket::Open(const unsigned short port, Address::eType family)
        {
            Close();

            int domain = family == Address::Ipv4 ? AF_INET : AF_INET6;

            // Create the socket
            if((m_Handle = socket(domain, SOCK_DGRAM, IPPROTO_UDP)) <= 0)
            {
                throw std::system_error(GetLastSystemError(), std::system_category(), "Failed to create socket.");
            }

            // Ipv6/Any
            if(family == Address::Any || family == Address::Ipv6)
            {
                const int v6only = family == Address::Any ? 0 : 1;

                // Enable/disable IPV6_V6ONLY
                if (setsockopt(m_Handle, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only)) != 0)
                {
                    Close();
                    throw std::system_error(GetLastSystemError(), std::system_category(), "Failed to enable/disable ipv6 only.");
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
                    throw std::system_error(GetLastSystemError(), std::system_category(), "Failed to bind socket to port " + std::to_string(port) +  ".");
                }
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
                    throw std::system_error(GetLastSystemError(), std::system_category(), "Failed to bind socket to port " + std::to_string(port) +  ".");
                }
            }


        }

        void UdpSocket::Close()
        {
            if(m_Handle != 0)
            {
                closesocket(m_Handle);
                m_Handle = 0;
            }
        }

        int UdpSocket::Send(const void * data, const size_t size, const SocketAddress & socketAddress)
        {
            /*
            ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
            */


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
            //ssize_t

            return 0;
        }

        SocketHandle UdpSocket::GetHandle()
        {
            return m_Handle;
        }

    }

}
