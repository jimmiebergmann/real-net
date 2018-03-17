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

#pragma once

#include <core/Socket.hpp>

namespace Net
{

    namespace Core
    {

        /**
        * @breif UDP Socket class.
        *
        */
        class UdpSocket : public Socket
        {

        public:

            /**
            * @breif Constructor.
            *
            */
            UdpSocket();

            /**
            * @breif Constructor.
            *        Initialize UDP socket by internally calling Open.
            *
            * @throw SystemException, containing socket error code.
            *
            */
            UdpSocket(const unsigned short port, Address::eType family = Address::Any);

            /**
            * @breif Destructor.
            *        Calls Close() at destruction.
            *
            */
            ~UdpSocket();

            /**
            * @breif Open UDP socket.
            *
            * @throw SystemException if failing, containing socket error code.
            *
            */
            void Open(const unsigned short port, const Address::eType family = Address::Any);
            void Open(const unsigned short port, const Address & address);

            /**
            * @breif Close open UDP socket.
            *
            */
            void Close();

            /**
            * @breif Send data.
            *
            * @param data Pointer to data buffer.
            * @param size Size of data.
            * @param socketAddress Socket address of received.
            *
            * @return Number of sent bytes.
            *
            */
            int Send(const void * data, const size_t size, const SocketAddress & socketAddress);

            /**
            * @breif Receive data.
            *
            * @param data Pointer to data buffer.
            * @param size Maximum size of data.
            * @param socketAddress Socket address of sender.
            *
            * @return Number of received bytes.
            *
            */
            int Receive(void * data, const size_t size, SocketAddress & socketAddress);

            /**
            * @breif Set blocking status.
            *
            * @throw SystemException if failing, containing socket error code.
            *
            */
            void SetBlocking(const bool status);

            /**
            * @breif Get address family.
            *
            */
            Address::eType GetFamily() const;

            /**
            * @breif Get socket handle.
            *
            */
            virtual SocketHandle GetHandle() const;

            /**
            * @breif Get socket address of socket.
            *
            */
            virtual SocketAddress GetSocketAddress() const;

        private:

            SocketHandle    m_Handle; ///< UDP socket handle.
            Address::eType  m_Family;
            SocketAddress   m_SocketAddress;
        };

    }

}
