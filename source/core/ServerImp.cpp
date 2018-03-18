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

        void ServerImp::InternalDisconnectPeer(Peer * pPeer, const bool triggerFunction, const bool sendResponse)
        {
            Core::SafeGuard sf_internalDisconnect(m_InternalDisconnectMutex);
            Core::SafeGuard sf_peers(m_PeersMutex);

            auto idPeerIt = m_IdPeers.find(pPeer->m_Id);
            if(idPeerIt == m_IdPeers.end())
            {
                return;
            }

            auto addressPeerIt = m_AddressPeers.find(pPeer->m_SocketAddress);

            if(sendResponse)
            {
                if(pPeer->m_State == PeerImp::Connected)
                {
                    const unsigned char disconnectData[4] =
                    {
                        Core::Packet::DisconnectionType, 1, 0, Core::Packet::DisconnectionTypeKicked
                    };

                    if(m_Socket.Send(disconnectData, 4, pPeer->m_SocketAddress) != 4)
                    {
                        std::cout << "Failed to send disconnect message." << std::endl;
                    }
                }
                else if(pPeer->m_State == PeerImp::Accepted || pPeer->m_State == PeerImp::Handshaking)
                {
                    const unsigned char rejectData[5] =
                    {
                        Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeKicked
                    };

                    if(m_Socket.Send(rejectData, 5, pPeer->m_SocketAddress) != 5)
                    {
                        std::cout << "Failed to send reject message." << std::endl;
                    }
                }
            }

            m_IdPeers.erase(idPeerIt);
            m_AddressPeers.erase(addressPeerIt);

            if(triggerFunction && pPeer->m_State == PeerImp::Connected)
            {
                AddTrigger(new Core::OnPeerDisconnectTrigger(pPeer));
            }
            pPeer->m_State = PeerImp::Disconnecting;

            m_PeerCleanup.Mutex.lock();
            m_PeerCleanup.Value.insert(pPeer);
            m_PeerCleanup.Mutex.unlock();

            m_ConnectionThreadSemaphore.NotifyOne();
        }

        bool ServerImp::InternalDisconnectPeer(const unsigned int id, const bool triggerFunction, const bool sendResponse)
        {
            Core::SafeGuard sf_internalDisconnect(m_InternalDisconnectMutex);
            Core::SafeGuard sf_peers(m_PeersMutex);

            auto idPeerIt = m_IdPeers.find(id);
            if(idPeerIt == m_IdPeers.end())
            {
                return false;
            }

            Peer * pPeer = idPeerIt->second;
            auto addressPeerIt = m_AddressPeers.find(pPeer->m_SocketAddress);

            if(sendResponse)
            {
                if(pPeer->m_State == PeerImp::Connected)
                {
                    const unsigned char disconnectData[4] =
                    {
                        Core::Packet::DisconnectionType, 1, 0, Core::Packet::DisconnectionTypeKicked
                    };

                    if(m_Socket.Send(disconnectData, 4, pPeer->m_SocketAddress) != 4)
                    {
                        std::cout << "Failed to send disconnect message." << std::endl;
                    }
                }
                else if(pPeer->m_State == PeerImp::Accepted || pPeer->m_State == PeerImp::Handshaking)
                {
                    const unsigned char rejectData[5] =
                    {
                        Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeKicked
                    };

                    if(m_Socket.Send(rejectData, 5, pPeer->m_SocketAddress) != 5)
                    {
                        std::cout << "Failed to send reject message." << std::endl;
                    }
                }
            }

            m_IdPeers.erase(idPeerIt);
            m_AddressPeers.erase(addressPeerIt);

            if(triggerFunction && pPeer->m_State == PeerImp::Connected)
            {
                AddTrigger(new Core::OnPeerDisconnectTrigger(pPeer));
            }
            pPeer->m_State = PeerImp::Disconnecting;

            m_PeerCleanup.Mutex.lock();
            m_PeerCleanup.Value.insert(pPeer);
            m_PeerCleanup.Mutex.unlock();

            m_ConnectionThreadSemaphore.NotifyOne();

            return true;
        }

        void ServerImp::QueueConnectionPacket(Packet * packet)
        {
            Core::SafeGuard sf(m_ConnectionPacketQueue);

            m_ConnectionPacketQueue.Value.push(packet);
            m_ConnectionThreadSemaphore.NotifyOne();
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

                auto it = m_IdPeers.find(m_LastPeerId);
                if(it == m_IdPeers.end())
                {
                    return m_LastPeerId;
                }
            }

            throw Exception("Should never reach end of GetNextPeerId().");
            return 0;
        }

    }

}
