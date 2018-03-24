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

#include <Peer.hpp>
#include <Server.hpp>

namespace Net
{
    Peer::Peer() :
        PeerImp()
    {
    }

    Peer::~Peer()
    {
    }

    unsigned short Peer::Id() const
    {
        return m_Id;
    }

    void Peer::Kick()
    {
        m_pServer->DisconnectPeerByPointer(this);
    }

    const SocketAddress & Peer::Address() const
    {
        return m_SocketAddress;
    }

    Peer::eState Peer::State() const
    {
        Core::SafeGuard safe_State(m_State);

        static const eState stateMap[4] =
        {
            Handshaking, Handshaking, Connected, Disconnected
        };

        return stateMap[m_State.Value];
    }

    Time Peer::Latency() const
    {
        Time time;
        m_pLatency->Get(time);
        return time;
    }

    Time Peer::ConnectedTime() const
    {
        Core::SafeGuard safe_State(m_State);

        if(m_State.Value != InternalConnected)
        {
            return Time::Zero;
        }

        Core::SafeGuard sf_clock(m_Clock);
        return m_Clock.Value.LapsedTime();
    }

    void Peer::SetTimeout(const Time & timeout)
    {
        //std::shared_ptr<Peer> peer(this);
        //m_pServer->SetPeerTimeout(peer, timeout);
    }

}
