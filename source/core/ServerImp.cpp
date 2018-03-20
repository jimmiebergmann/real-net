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
            m_OnPeerPreConnect(nullptr),
            m_OnPeerConnect(nullptr),
            m_OnPeerDisconnect(nullptr)
        {
        }

        void ServerImp::InternalDisconnectPeer(std::shared_ptr<Peer> peer, const bool triggerFunction, const bool sendResponse)
        {
            if(!peer)
            {
                return;
            }

            Core::SafeGuard sf_peers(m_PeersMutex);

            auto addressPeerIt = m_Peers.find(peer->m_SocketAddress);
            if(addressPeerIt == m_Peers.end())
            {
                return;
            }

            if(sendResponse)
            {
                if(peer->m_State == PeerImp::Connected)
                {
                    const unsigned char disconnectData[4] =
                    {
                        Core::Packet::DisconnectionType, 1, 0, Core::Packet::DisconnectionTypeKicked
                    };

                    if(m_Socket.Send(disconnectData, 4, peer->m_SocketAddress) != 4)
                    {
                        std::cout << "Failed to send disconnect message." << std::endl;
                    }
                }
                else if(peer->m_State == PeerImp::Accepted || peer->m_State == PeerImp::Handshaking)
                {
                    const unsigned char rejectData[5] =
                    {
                        Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeKicked
                    };

                    if(m_Socket.Send(rejectData, 5, peer->m_SocketAddress) != 5)
                    {
                        std::cout << "Failed to send reject message." << std::endl;
                    }
                }
            }

            m_PeerIds.erase(peer->m_Id);
            m_Peers.erase(addressPeerIt);
            auto handshakingPeerIt = m_HandshakingPeers.find(peer);
            if(handshakingPeerIt != m_HandshakingPeers.end())
            {
                m_HandshakingPeers.erase(handshakingPeerIt);
            }

            if(triggerFunction && (peer->m_State == PeerImp::Connected || peer->m_State == PeerImp::Accepted))
            {
                AddTrigger(new Core::OnPeerDisconnectTrigger(peer));
            }
            peer->m_State = PeerImp::Disconnecting;
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

        void ServerImp::AddTrigger(Trigger * trigger)
        {
            if(trigger == nullptr)
            {
                throw Exception("Passed nullptr to AddTrigger.");
            }

            Core::SafeGuard sf(m_TriggerQueue);
            m_TriggerQueue.Value.push(trigger);
            m_TriggerThreadSemaphore.NotifyOne();
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
