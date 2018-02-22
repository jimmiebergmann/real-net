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

#include <Address.hpp>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <netdb.h>

namespace Net
{

    static const unsigned char g_ZeroArray16[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    // Address class.
    const Address Address::None;

    Address::Address(const eType type)
    {
        m_Type = type;
        Zero();
    }

    Address::Address(const std::string & hostname, const eType family)

    {
        Hostname(hostname, family);
    }

    Address::Address(const unsigned char Ipv4_1, const unsigned char Ipv4_2, const unsigned char Ipv4_3, const unsigned char Ipv4_4)
    {
        m_Type = Ipv4;
        m_Bytes[0] = Ipv4_1;
        m_Bytes[1] = Ipv4_2;
        m_Bytes[2] = Ipv4_3;
        m_Bytes[3] = Ipv4_4;
    }

    Address::Address(const std::initializer_list<unsigned char> & ipv6List)
    {
        m_Type = Ipv6;
        if(ipv6List.size() != 16)
        {
            Zero();
            return;
        }

        std::copy(ipv6List.begin(), ipv6List.end(), m_Bytes);
    }

    bool Address::Hostname(const std::string & hostname, const eType family)
    {
        // Get address info by hints.
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        if(family != Any)
        {
            hints.ai_family = family == Ipv4 ? AF_INET : AF_INET6;
        }
        hints.ai_socktype = SOCK_STREAM;

        int status = 0;
        struct addrinfo * pResult = NULL;
        struct addrinfo * pCurrent = NULL;
        if ((status = getaddrinfo( hostname.c_str(), NULL , &hints , &pResult)) != 0)
        {
            Zero();
            return false;
        }

        // loop through all the results and find the best result.
        eType bestype = Any;

        for(pCurrent = pResult; pCurrent != NULL; pCurrent = pCurrent->ai_next)
        {
            if(pCurrent->ai_family == AF_UNSPEC)
            {
                Zero();
                return false;
            }
            else if(pCurrent->ai_family == AF_INET)
            {
                bestype = Ipv4;
                struct sockaddr_in * sockaddr_ipv4 = (struct sockaddr_in *) pCurrent->ai_addr;
                *(unsigned int *)m_Bytes = sockaddr_ipv4->sin_addr.s_addr;
                memset(m_Bytes + 4, 0 , 12);
            }
            else
            {
                bestype = Ipv6;
                struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *) pCurrent->ai_addr;
                memcpy(m_Bytes, sockaddr_ipv6->sin6_addr.s6_addr, 16);
            }

            // We found the best type, break.
            if(bestype == Ipv6)
            {
                break;
            }
        }

        if(bestype == Any)
        {
            Zero();
            return false;
        }

        freeaddrinfo(pResult);
        m_Type = bestype;
        return true;
    }

    void Address::Zero()
    {
        m_Type = Any;
        memset(m_Bytes, 0 , 16);
    }

    void Address::Loopback(const eType family)
    {
        m_Type = family;
        if(m_Type == Ipv4)
        {
            m_Bytes[0] = 127;
            m_Bytes[1] = 0;
            m_Bytes[2] = 0;
            m_Bytes[3] = 1;
            memset(m_Bytes + 4, 0 , 12);
            return;
        }
        if(m_Type == Ipv6)
        {
            m_Bytes[15] = 1;
            memset(m_Bytes, 0 , 15);
            return;
        }
    }

    void Address::SetByte(const size_t index, const unsigned char byte)
    {
        if(index >= 16)
        {
            return;
        }

        m_Bytes[index] = byte;
    }

    Address::eType Address::Type() const
    {
        return m_Type;
    }

    bool Address::IsLoopback() const
    {
        if(m_Type == Ipv4)
        {
            if(m_Bytes[0] != 127 || m_Bytes[1] != 0 || m_Bytes[2] != 0 || m_Bytes[3] != 1 ||
               memcmp(m_Bytes + 4, g_ZeroArray16, 12) != 0)
            {
                return false;
            }

            return true;
        }
        if(m_Type == Ipv6)
        {
            if(m_Bytes[15] != 1 || memcmp(m_Bytes, g_ZeroArray16, 15) != 0)
            {
                return false;
            }

            return true;
        }

        return false;
    }

    bool Address::IsZero() const
    {
        return memcmp(m_Bytes, g_ZeroArray16, 16) == 0;
    }

    unsigned char const (& Address::Bytes() const)[16]
    {
        return m_Bytes;
    }

    std::string Address::String() const
    {
        if(m_Type == Ipv4)
        {
            std::string ret = "";
            for(size_t i = 0; i < 4; i++)
            {
                ret += std::to_string((int)m_Bytes[i]);
                if(i == 3)
                {
                    break;
                }
                ret += ".";
            }
            return ret;
        }

        if(m_Type == Ipv6)
        {
            std::stringstream stream;
            for(size_t i = 0; i < 16; i++)
            {
                stream << std::hex << static_cast<int>(m_Bytes[i]);
                if(i == 15)
                {
                    break;
                }
                stream << ":";
            }
            return stream.str();
        }

        return "0";
    }


    // Socket address class.
    SocketAddress::SocketAddress(const Address & address, const unsigned short port) :
        m_Address(address),
        m_Port(port)
    {
    }

    Address & SocketAddress::AddressRef()
    {
        return m_Address;
    }

    unsigned short & SocketAddress::PortRef()
    {
        return m_Port;
    }

}
