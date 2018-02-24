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

#include <core/SocketSelector.hpp>

namespace Net
{

    namespace Core
    {

        SocketSelector::SocketSelector() :
            m_StopHandle(0)
        {
            Start();
        }

        SocketSelector::~SocketSelector()
        {
            Stop();
        }

        bool SocketSelector::Select(Socket * socket, const Time & timeout)
        {
            if(socket == nullptr)
            {
                return false;
            }
            SocketHandle handle = socket->GetHandle();
            if(handle == 0)
            {
                return false;
            }

            fd_set readSet;
            int highest = 0;
            {
                std::lock_guard<std::mutex> sf(m_Mutex);

                if(m_Stopped)
                {
                    return false;
                }

                FD_ZERO(&readSet);
                FD_SET(m_StopHandle, &readSet);
                FD_SET(handle, &readSet);

                highest = m_StopHandle > handle ? m_StopHandle : socket->GetHandle();
            }

            struct timeval time;
            struct timeval * pTimeout = NULL;
            if(timeout != Time::Infinite)
            {
                time.tv_sec = timeout.AsMicroseconds() / 1000000ULL;
                time.tv_usec = timeout.AsMicroseconds() % 1000000ULL;
                pTimeout = &time;
            }

            if(select(highest + 1, &readSet, NULL, NULL, pTimeout) == -1)
            {
                return false;
            }

            // Selector has been stopped.
            if(m_Stopped || FD_ISSET(handle, &readSet) == false)
            {
                return false;
            }

            return true;
        }

        void SocketSelector::Start()
        {
            {
                std::lock_guard<std::mutex> sf(m_Mutex);

                if(m_StopHandle == 0)
                {
                    m_StopHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                }

                m_Stopped = false;
            }

        }

        void SocketSelector::Stop()
        {
            {
                std::lock_guard<std::mutex> sf(m_Mutex);

                m_Stopped = true;

                if(m_StopHandle)
                {
                    closesocket(m_StopHandle);
                    m_StopHandle = 0;
                }
            }
        }

    }

}
