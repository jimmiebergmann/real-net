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

#include <core/UdpSocket.hpp>
#include <Time.hpp>
#include <mutex>
#include <atomic>

namespace Net
{

    namespace Core
    {

        /**
        * @breif Socket selector class.
        *
        */
        class SocketSelector
        {

        public:

            /**
            * @breif Constructor.
            *        Starting selector if socket != nullptr.
            *
            */
            SocketSelector(UdpSocket * socket = nullptr);

            /**
            * @breif Destructor.
            *
            */
            ~SocketSelector();

            /**
            * @breif Wait for data to read from socket, by given timeout.
            *        It's possible to break the wait by calling Stop().
            *
            * @return True if new data is available to read from socket,
            *         false if timeout is reached or stopped via Stop().
            *
            */
            bool Select(const Time & timeout = Time::Infinite);

            /**
            * @breif Wait for data to read from socket, by given timeout.
            *        It's possible to break the wait by calling Stop().
            *
            */
            void Start(UdpSocket * socket);

            /**
            * @breif Wait for data to read from socket.
            *        It's possible to break the wait by calling Stop().
            *
            */
            void Stop();

        private:

            UdpSocket * m_pSocket;
            std::mutex          m_Mutex;        ///< Socket handle mutex.
            std::atomic<bool>   m_Stopped;      ///< Stopped flag.

        };

    }

}
