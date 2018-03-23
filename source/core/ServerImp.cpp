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

#include <core/ServerImp.hpp>
#include <iostream>

namespace Net
{

    namespace Core
    {

        ServerImp::ServerImp() :
            m_Hosted(false),
            m_Stopping(false),
            m_LastPeerId(0),
            m_PacketPool(10),
            m_TriggerQueue(nullptr, true),
            m_OnPeerPreConnect(nullptr),
            m_OnPeerConnect(nullptr),
            m_OnPeerDisconnect(nullptr)
        {
        }

        void ServerImp::DisconnectPeerByPointer(Peer * peer)
        {
            std::map<SocketAddress, std::shared_ptr<Peer>>::iterator peerIt;

            {
                peerIt = m_Peers.find(peer->m_SocketAddress);
                if(peerIt == m_Peers.end())
                {
                    throw Exception("Cannot find peer by raw pointer.");
                    return;
                }

            }

            DisconnectPeer(peerIt->second);

        }

        void ServerImp::QueueConnectionPacket(Packet * packet)
        {
            Core::SafeGuard sf(m_ConnectionPacketQueue);

            m_ConnectionPacketQueue.Value.push(packet);
            m_ConnectionThreadSemaphore.NotifyOne();
        }

        Packet * ServerImp::GetConnectionPacket()
        {
            Core::SafeGuard sf_peer(m_ConnectionPacketQueue);

            Packet * pPacket = nullptr;

            if(m_ConnectionPacketQueue.Value.size())
            {
                pPacket = m_ConnectionPacketQueue.Value.front();
                m_ConnectionPacketQueue.Value.pop();
            }

            return pPacket;
        }

        unsigned int ServerImp::GetNextPeerId()
        {
            while(1)
            {
                m_LastPeerId++;

                auto it = m_PeerIds.find(m_LastPeerId);
                if(it == m_PeerIds.end())
                {
                    m_PeerIds.insert(m_LastPeerId);
                    return m_LastPeerId;
                }
            }

            throw Exception("Should never reach end of GetNextPeerId().");
            return 0;
        }

    }

}
