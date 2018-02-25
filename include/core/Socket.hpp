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

#include <core/Build.hpp>
#if defined(REALNET_PLATFORM_WINDOWS)
    #include <core/platform/WindowsHeaders.hpp>
#elif defined(REALNET_PLATFORM_LINUX)
    #include <core/platform/LinuxHeaders.hpp>
#endif

#include <Address.hpp>

namespace Net
{

    namespace Core
    {

    #if defined(REALNET_PLATFORM_WINDOWS)
        typedef SOCKET SocketHandle;

        // Intiialize winsock initializer.
        struct WinsockInitializer
        {
            WinsockInitializer()
            {
                WSADATA wsaData;
                if( WSAStartup(MAKEWORD(2,2), &wsaData))
                {
                    throw std::runtime_error("Failed to initialize winsock.");
                }
            }
        };
        static WinsockInitializer RealnetWinsockInitializer;

    #elif defined(REALNET_PLATFORM_LINUX)
        typedef unsigned int SocketHandle;
    #endif

        /**
        * @breif Socket base class.
        *
        */
        class Socket
        {

        public:

            /**
            * @breif Get socket handle from socket class.
            *
            */
            virtual SocketHandle GetHandle() const = 0;

            /**
            * @breif Get socket address of socket.
            *
            */
            virtual SocketAddress GetSocketAddress() const = 0;

        };

    }

}
