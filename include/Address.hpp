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

/*
Resources:
 - https://stackoverflow.com/questions/6027989/how-to-set-sockaddr-in6sin6-addr-byte-order-to-network-byte-order
 - https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
 - http://man7.org/linux/man-pages/man3/getaddrinfo.3.html
 - https://msdn.microsoft.com/en-us/library/windows/desktop/ms738520(v=vs.85).aspx
*/

#pragma once

#include <core/Build.hpp>
#include <string>
#include <initializer_list>

namespace Net
{

    /**
    * @breif Address class containing information about ipv4 or ipv6 addresses.
    *
    */
    class Address
    {

    public:

        enum eType
        {
            Any,
            Ipv4,
            Ipv6
        };

        /**
        * @breif Constructor.
        *
        * If ipv6List.size() != 16, address is initialized as zero.
        *
        * @param hostname Initialize address by hostname.
        * @param Ipv4_* Initialize address as ipv4.
        * @param ipv6List Initialize address as ipv6.
        * @param ipv6Bytes Pointer to 16 bytes. Initializes as ipv6.
        *
        */
        Address(const eType type = Any);
        Address(const std::string & hostname, const eType family = Any);
        Address(const unsigned char Ipv4_1, const unsigned char Ipv4_2, const unsigned char Ipv4_3, const unsigned char Ipv4_4);
        Address(const std::initializer_list<unsigned char> & ipv6List);
        Address(const unsigned char * ipv6Bytes);

        /**
        * @breif Initialize address.
        *
        * @param hostname Name of host.
        * @param Ipv4_* Initialize address as ipv4.
        * @param ipv6List Initialize address as ipv6.
        * @param ipv6Bytes Pointer to 16 bytes. Initializes as ipv6.
        *
        * @return True if succeeded, false if hostname is unknown, or ipv6List.size() != 16.
        *
        */
        bool Set(const std::string & hostname, const eType family = Any);
        void Set(const unsigned char Ipv4_1, const unsigned char Ipv4_2, const unsigned char Ipv4_3, const unsigned char Ipv4_4);
        bool Set(const std::initializer_list<unsigned char> & ipv6List);
        void Set(const unsigned char * ipv6Bytes);

        /**
        * @breif Clear address.
        *
        */
        void SetZero();

        /**
        * @breif Initialize address as loopback.
        *
        * @param family Ip version of loopback address.
        *               "Any" will give no result.
        *
        */
        void SetLoopback(const eType family = Ipv6);

        /**
        * @breif Set individual bytes.
        *
        * @param index Index of byte to set.
        * @param byte new byte value.
        *
        */
        void SetByte(const size_t index, const unsigned char byte);

        /**
        * @breif Get type of address.
        *
        */
        eType GetType() const;

        /**
        * @breif Check if address is loopback.
        *
        */
        bool IsLoopback() const;

        /**
        * @breif Check if address is zero.
        *
        */
        bool IsZero() const;

        /**
        * @breif Get address bytes.
        *
        * @return Reference to byte array.
        *
        */
        unsigned char const (& GetBytes() const)[16];

        /**
        * @breif Get address as readable string.
        *
        */
        std::string GetAsString() const;

        /**
        * @breif Compare with another address.
        *
        */
        bool operator == (const Address & address) const;

        /**
        * @breif The "Zero" address contains only "zero" bytes.
        *
        */
        static const Address Zero;

        /**
        * @breif Return loopback address of given family.
        *        Returns Zero if family = Any.
        *
        */
        static const Address & Loopback(const eType family);

    private:

        eType           m_Type;         ///< Type of address.
        unsigned char   m_Bytes[16];    ///< Bytes describing address.

    };


    /**
    * @breif Socket address combines an address and a port.
    *
    */
    struct SocketAddress
    {

        /**
        * @breif Constructor.
        *
        * @param address Ip address.
        * @param port Port number.
        *
        */
        SocketAddress(const Address & address = Address::Zero, const unsigned short port = 0);

        Address         Ip;     ///< Socket address.
        unsigned short  Port;   ///< Socket port.

    };

}
