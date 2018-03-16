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


// Define platform
#if defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 ) || defined( __WIN64__ )
	#define REALNET_PLATFORM_WINDOWS

	#include <core/platform/WindowsHeaders.hpp>
#elif defined(linux) || defined(__linux)
	#define REALNET_PLATFORM_LINUX

    #include <core/platform/LinuxHeaders.hpp>

	#define REALNET_ENDIAN_16(x) ((1!=htonl(1)) ? (x) : bswap_16(x))
    #define REALNET_ENDIAN_32(x) ((1!=htonl(1)) ? (x) : bswap_32(x))
    #define REALNET_ENDIAN_64(x) ((1!=htonl(1)) ? (x) : bswap_64(x))

#else
	#error No platform defined.
#endif

// Define test friend.
#ifndef REALNET_TEST_FRIEND
#define REALNET_TEST_FRIEND
#endif

#include <Exception.hpp>


namespace Net
{

    // Typedef the data types used in real-net for entity and groups id.
    typedef unsigned short  EntityIdType;
    typedef unsigned int    GroupIdType;

    namespace Core
    {

        /**
        * @breif Get socket handle from socket class.
        *
        */
        size_t GetLastSystemError();
    }
}
