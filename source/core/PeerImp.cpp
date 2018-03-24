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

#include <core/PeerImp.hpp>
#include <Peer.hpp>

namespace Net
{

    namespace Core
    {

        PeerImp::PeerImp() :
            m_pServer(nullptr),
            m_State(InternalHandshaking),
            m_Id(0),
            m_Timeout(0),
            m_pLatency(nullptr)
        {
        }

        PeerImp::~PeerImp()
        {
            if(m_pLatency)
            {
                delete m_pLatency;
            }
        }

        void PeerImp::Initialize(Server * pServer, const unsigned short id, const SocketAddress & socketAddress,
                                 const Time & timeout, const size_t latencySamples)
        {
            m_pServer = pServer;
            m_Id = id;
            m_SocketAddress = socketAddress;
            m_Timeout = timeout.AsMicroseconds();
            m_pLatency = new Latency(latencySamples);
        }

        void PeerImp::SetState(const eInternalState state)
        {
            Core::SafeGuard safe_State(m_State);

            if(m_State.Value >= state)
            {
                return;
            }

            m_State.Value = state;
        }


    }
}
