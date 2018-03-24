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

        void ServerImp::DisconnectPeer(std::shared_ptr<Peer> peer, const Peer::Disconnect::eReason reason,
                                       const bool triggerFunction, const bool sendResponse)
        {
            Core::SafeGuard sf_peerDisconnect(m_PeerDisconnectMutex);

            if(peer->m_State == Core::PeerImp::InternalDisconnected)
            {
                return;
            }

            {
                Core::SafeGuard sf_peers(m_PeersMutex); ///< Mutex lock

                // Erase peer
                auto peerIt = m_Peers.find(peer->m_SocketAddress);
                if(peerIt != m_Peers.end())
                {
                    m_Peers.erase(peerIt);
                    m_PeerIds.erase(peer->m_Id);
                }

                peer->m_State = Core::PeerImp::InternalDisconnected;
            }

            if(triggerFunction || sendResponse)
            {
                m_TriggerQueue.Push(new Core::OnPeerDisconnectTrigger(peer, reason, triggerFunction, sendResponse));
            }
        }

        ServerImp::ServerImp() :
            m_Hosted(false),
            m_Stopping(false),
            m_LastPeerId(0),
            m_PacketPool(10),
            m_ConnectionPacketQueue(nullptr, true),
            m_TriggerQueue(nullptr, true),
            m_OnPeerPreConnect(nullptr),
            m_OnPeerConnect(nullptr),
            m_OnPeerDisconnect(nullptr)
        {
        }

        ServerImp::PeerMap::iterator ServerImp::DisconnectPeerByIterator(PeerMap::iterator it, const Peer::Disconnect::eReason reason)
        {
            Core::SafeGuard sf_peerDisconnect(m_PeerDisconnectMutex);

            auto peer = it->second;

            if(peer->m_State == Core::PeerImp::InternalDisconnected)
            {
                return it;
            }

            auto returnIt = m_Peers.erase(it);
            m_PeerIds.erase(peer->m_Id);
            peer->m_State = Core::PeerImp::InternalDisconnected;

            m_TriggerQueue.Push(new Core::OnPeerDisconnectTrigger(peer, reason, true, true));

            return returnIt;
        }

        void ServerImp::DisconnectPeerByPointer(Peer * peerPointer, const Peer::Disconnect::eReason reason)
        {
            std::shared_ptr<Peer> peer;

            {
                Core::SafeGuard sf_peers(m_PeersMutex);

                auto peerIt = m_Peers.find(peerPointer->m_SocketAddress);
                if(peerIt == m_Peers.end())
                {
                    throw Exception("Cannot find peer by raw pointer: " + std::to_string(peerPointer->m_Id));
                    return;
                }

                peer = peerIt->second;

            }

            DisconnectPeer(peer, reason, true, true);

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
