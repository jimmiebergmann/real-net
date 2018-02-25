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
#include <core/Safe.hpp>

namespace Net
{

    namespace Core
    {

        SocketSelector::SocketSelector(UdpSocket * socket) :
            m_pSocket(nullptr),
            m_Stopped(true)
        {
            if(socket)
            {
                Start(socket);
            }
        }

        SocketSelector::~SocketSelector()
        {
            Stop();
        }

        bool SocketSelector::Select(const Time & timeout)
        {
            if(m_pSocket == nullptr)
            {
                return false;
            }

            const SocketHandle handle = m_pSocket->GetHandle();
            if(handle == 0)
            {
                return false;
            }

            fd_set readSet;
            {
                SafeGuard sf(m_Mutex);

                if(m_Stopped)
                {
                    return false;
                }

                FD_ZERO(&readSet);
                FD_SET(handle, &readSet);
            }

            struct timeval time;
            struct timeval * pTimeout = NULL;
            if(timeout != Time::Infinite)
            {
                time.tv_sec = timeout.AsMicroseconds() / 1000000ULL;
                time.tv_usec = timeout.AsMicroseconds() % 1000000ULL;
                pTimeout = &time;
            }

            int ret = 0;
            if((ret = select(handle + 1, &readSet, NULL, NULL, pTimeout)) == -1)
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

        void SocketSelector::Start(UdpSocket * socket)
        {
            SafeGuard sf(m_Mutex);

            if(m_Stopped == true)
            {
                if(socket == nullptr)
                {
                    return;
                }

                m_pSocket = socket;
            }

            m_Stopped = false;
        }

        void SocketSelector::Stop()
        {
            SafeGuard sf(m_Mutex);

            if(m_Stopped == false)
            {
                m_Stopped = true;
                if(m_pSocket)
                {
                    m_pSocket->Send(NULL, 0, m_pSocket->GetSocketAddress());
                }
            }
        }

    }

}
