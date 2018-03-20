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

#include <Server.hpp>
#include <core/Safe.hpp>
#include <core/Packet.hpp>
#include <Clock.hpp>
#include <cstring>
#include <iostream>

namespace Net
{

    Server::Server()
    {
    }

    Server::~Server()
    {
        Stop();
    }

    void Server::Host(const unsigned short port, const size_t maxConnections)
    {
        m_Settings.port = port;
        m_Settings.maxConnections = maxConnections;
        Host(m_Settings);
    }

    void Server::Host(const unsigned short port, const size_t maxConnections, const Settings & settings)
    {
        if(port != 0)
        {
            m_Settings.port = port;
        }
        if(maxConnections != 0)
        {
            m_Settings.maxConnections = maxConnections;
        }
        Host(m_Settings);
    }

    void Server::Host(const Settings & settings)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_Hosted == true)
        {
            throw Exception("Server is already hosted.");
            return;
        }
        if(m_Stopping == true)
        {
            throw Exception("Server is currently stopping.");
            return;
        }

        m_Socket.Open(settings.port, settings.host.GetType());
        m_SocketSelector.Start(&m_Socket);

        m_Settings = settings;

        // Receive thead
        Core::Semaphore receiveThreadStarted;
        m_ReceiveThread = std::thread([this, &receiveThreadStarted]()
        {
            Core::Packet * pPacket = m_PacketPool.Get();

            receiveThreadStarted.NotifyOne();

            while(m_Stopping == false)
            {
                if(m_SocketSelector.Select(Microseconds(500000ULL)) == false)
                {
                    continue;
                }

                int receiveSize = m_Socket.Receive(pPacket->data, Core::Packet::MaxPacketSize, pPacket->address);

                if(receiveSize < 0)
                {
                    throw Exception("Receive thread: Error while receiving data: " + std::to_string(Core::GetLastSystemError()));
                    break;
                }

                if(receiveSize < 3)
                {
                    continue;
                }

                pPacket->receiveTime = Clock::SystemTime();

                const unsigned char packetType = pPacket->data[0];

                // Find peer
                {
                    Core::SafeGuard sf_peers(m_PeersMutex);

                    std::shared_ptr<Peer> peer;

                    // Find peer
                    auto peerIt = m_Peers.find(pPacket->address);
                    if(peerIt != m_Peers.end())
                    {
                        // New peers or not yet connected peers are only allowed to send connection packets.
                        if(peerIt->second->m_State != Core::PeerImp::Connected && packetType != Core::Packet::ConnectionType)
                        {
                            continue;
                        }

                        pPacket->peer = peerIt->second;
                    }
                }

                pPacket->size = static_cast<size_t>(receiveSize);
                pPacket->SerializeSequenceNumber();

                // Handle peer data.
                switch(packetType)
                {
                    case Core::Packet::ConnectionType:
                    case Core::Packet::DisconnectionType:
                    {
                        QueueConnectionPacket(pPacket);
                    }
                    break;
                    case Core::Packet::SynchronizationType:
                    {
                    }
                    break;
                    case Core::Packet::AcknowledgementType:
                    {
                    }
                    break;
                    case Core::Packet::MessageType:
                    {
                    }
                    break;
                    default:
                        break;
                };

                pPacket = m_PacketPool.Get();

            }
        });

        // Reliable thread.
        Core::Semaphore reliableThreadStarted;
        m_ReliableThread = std::thread([&reliableThreadStarted]()
        {
            reliableThreadStarted.NotifyOne();

        });

        // Connection thread.
        Core::Semaphore connectionThreadStarted;
        m_ConnectionThread = std::thread([this, &connectionThreadStarted]()
        {
            // Pre-allocated respones.
            static const size_t acceptReponseSize = 20;
            static const size_t fullReponseSize = 5;
            static const size_t ackReponseSize = 3;
            static unsigned char acceptReponse[acceptReponseSize] =
            {
                Core::Packet::ConnectionType, 0, 0, Core::Packet::ConnectionTypeAccept,
                0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0
            };
            static unsigned char fullReponse[fullReponseSize] =
            {
                 Core::Packet::ConnectionType, 0, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeFull
            };
            static unsigned char ackReponse[ackReponseSize] =
            {
                 Core::Packet::AcknowledgementType, 0, 0
            };

            connectionThreadStarted.NotifyOne();

            Time waitTime = Time::Infinite;

            while(m_Stopping == false)
            {
                m_ConnectionThreadSemaphore.WaitFor(waitTime);

                if(m_Stopping)
                {
                    return;
                }

                // Handle connection packets
                Core::Packet * pPacket = GetConnectionPacket();
                std::shared_ptr<Peer> peer;
                if(pPacket)
                {
                    peer = pPacket->peer;

                    // Do not care about disconnecting peers
                    if(peer && peer->m_State == Core::PeerImp::Disconnecting)
                    {
                        m_PacketPool.Return(pPacket);
                        pPacket = nullptr;
                    }
                }

                if(pPacket)
                {
                    switch(pPacket->data[0])
                    {
                        // Connection packet.
                        case Core::Packet::ConnectionType:
                        {
                            switch(pPacket->data[3])
                            {
                                // Connection init packet.
                                case Core::Packet::ConnectionTypeInit:
                                {
                                    if(pPacket->size != 4 || pPacket->sequence != 0)
                                    {
                                        break;
                                    }

                                    // Send accpet packet, since it's already been accepted.
                                    if(peer && (peer->m_State == Core::PeerImp::Accepted || peer->m_State == Core::PeerImp::Connected))
                                    {
                                        const unsigned long long receiveTime = REALNET_ENDIAN_64(pPacket->receiveTime.AsMicroseconds());
                                        memcpy(acceptReponse + 4, &receiveTime, 8);
                                        const unsigned long long sendTime = REALNET_ENDIAN_64(Clock::SystemTime().AsMicroseconds());
                                        memcpy(acceptReponse + 12, &sendTime, 8);

                                        if(m_Socket.Send(acceptReponse, acceptReponseSize, peer->m_SocketAddress) != acceptReponseSize)
                                        {
                                            std::cout << "Failed to send accept message." << std::endl;
                                            InternalDisconnectPeer(peer, false, false);
                                            break;
                                        }
                                    }
                                    // New connection.
                                    else
                                    {
                                        Core::SafeGuard sf_peers(m_PeersMutex);

                                        if(m_Peers.size() >= m_Settings.maxConnections)
                                        {
                                            if(m_Socket.Send(fullReponse, fullReponseSize, pPacket->address) != fullReponseSize)
                                            {
                                                std::cout << "Failed to send full message." << std::endl;
                                            }
                                            break;
                                        }

                                        if(!peer)
                                        {
                                            const unsigned int peerId = GetNextPeerId();
                                            peer = std::make_shared<Peer>();
                                            peer->Initialize(peerId, pPacket->address, m_Settings.latencySamples);
                                            m_Peers.insert({pPacket->address, peer});
                                        }

                                        AddTrigger(new Core::OnPeerPreConnectTrigger(peer, pPacket->receiveTime));
                                    }
                                }
                                break;
                                case Core::Packet::ConnectionTypeAccept:
                                {
                                    if(pPacket->size != 20 || !peer || pPacket->sequence != 1)
                                    {
                                        break;
                                    }

                                    if(peer->m_State == Core::PeerImp::Connected)
                                    {
                                        // Send ack.
                                        ackReponse[1] = 1;
                                        ackReponse[2] = 0;
                                        if(m_Socket.Send(ackReponse, ackReponseSize, peer->m_SocketAddress) != ackReponseSize)
                                        {
                                            std::cout << "Failed to send acknowledgement message." << std::endl;
                                            InternalDisconnectPeer(peer, true, true);
                                            break;
                                        }
                                    }
                                    else if(peer->m_State == Core::PeerImp::Accepted)
                                    {
                                        // Check if the timed out.
                                        {
                                            Core::SafeGuard sf_peers(m_PeersMutex);

                                            auto handshakingPeerIt = m_HandshakingPeers.find(peer);
                                            if(handshakingPeerIt == m_HandshakingPeers.end())
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                m_HandshakingPeers.erase(handshakingPeerIt);
                                            }
                                        }

                                        // Caluclate initial latency.
                                        peer->m_Clock.Mutex.lock();
                                        const Time serverDelay = pPacket->receiveTime - peer->m_Clock.Value.StartTime();
                                        peer->m_Clock.Mutex.unlock();

                                        unsigned long long tempPeerTime = 0;
                                        memcpy(&tempPeerTime, pPacket->data + 4, 8);
                                        tempPeerTime = REALNET_ENDIAN_64(tempPeerTime);

                                        unsigned long long tempPeerDelay = 0;
                                        memcpy(&tempPeerDelay, pPacket->data + 12, 8);
                                        tempPeerDelay = REALNET_ENDIAN_64(tempPeerDelay);

                                        const Time peerDelay = Microseconds(tempPeerDelay);

                                        // Incorrect delay from peer.
                                        if(peerDelay > serverDelay)
                                        {
                                            InternalDisconnectPeer(peer, true, true);
                                            break;
                                        }

                                        // Send ack.
                                        ackReponse[1] = 1;
                                        ackReponse[2] = 0;
                                        if(m_Socket.Send(ackReponse, ackReponseSize, peer->m_SocketAddress) != ackReponseSize)
                                        {
                                            std::cout << "Failed to send acknowledgement message." << std::endl;
                                            InternalDisconnectPeer(peer, true, true);
                                            break;
                                        }

                                        const Time latency = serverDelay - peerDelay;
                                        peer->m_pLatency->Add(latency);

                                        // Peer is now connected.
                                        peer->m_State = Core::PeerImp::Connected;

                                        peer->m_Clock.Mutex.lock();
                                        peer->m_Clock.Value.Start();
                                        peer->m_Clock.Mutex.unlock();

                                        AddTrigger(new Core::OnPeerConnectTrigger(peer));
                                    }
                                }
                                break;
                                case Core::Packet::ConnectionTypeReject:
                                {
                                    if(pPacket->size != 4 || !peer || peer->m_State != Core::PeerImp::Accepted || pPacket->sequence != 1)
                                    {
                                        break;
                                    }

                                    // Send ack.
                                    ackReponse[1] = 1;
                                    ackReponse[2] = 0;
                                    if(m_Socket.Send(ackReponse, ackReponseSize, peer->m_SocketAddress) != ackReponseSize)
                                    {
                                        std::cout << "Failed to send acknowledgement message." << std::endl;
                                        break;
                                    }

                                    InternalDisconnectPeer(peer, true, false);
                                }
                                break;
                                default:
                                    break;
                            };
                        }
                        break;
                        // Disconnection packet.
                        case Core::Packet::DisconnectionType:
                        {
                            if(pPacket->size != 3)
                            {
                                break;
                            }

                            // Send ack.
                            const unsigned short sequence = REALNET_ENDIAN_16(pPacket->sequence);
                            memcpy(ackReponse + 1, &sequence, 2);
                            if(m_Socket.Send(ackReponse, ackReponseSize, pPacket->address) != ackReponseSize)
                            {
                                std::cout << "Failed to send acknowledgement message." << std::endl;
                                break;
                            }

                            InternalDisconnectPeer(peer, true, false);
                        }
                        break;
                        default:
                            break;
                    };

                    m_PacketPool.Return(pPacket);

                }

                Time lowestHandshakeTimeout = Time::Infinite;

                // Handle handshake timeouts.
                for(auto it = m_HandshakingPeers.begin(); it != m_HandshakingPeers.end();)
                {
                    m_PeersMutex.lock(); ///< Mutex lock.

                    Core::SafeGuard sf_peerClock((*it)->m_Clock);

                    Time lapsed = (*it)->m_Clock.Value.LapsedTime();
                    if(lapsed >= m_Settings.handshakeTimeout)
                    {
                        std::shared_ptr<Peer> peer = *it;

                        it = m_HandshakingPeers.erase(it);

                        m_PeersMutex.unlock();  ///< Mutex unlock.
                        InternalDisconnectPeer(peer, true, true);
                        continue;
                    }
                    else
                    {
                        const Time timeLeft = m_Settings.handshakeTimeout - lapsed;
                        if(timeLeft < lowestHandshakeTimeout)
                        {
                            lowestHandshakeTimeout = timeLeft;
                        }

                        ++it;
                    }

                    m_PeersMutex.unlock();  ///< Mutex unlock.
                }

                waitTime = lowestHandshakeTimeout;
                if(waitTime != Time::Infinite)
                {
                    waitTime += Milliseconds(1);
                }

                // Handle connection timeouts.
                // ...

            }
        });

        // Trigger queue.
        Core::Semaphore triggerThreadStarted;
        m_TriggerThread = std::thread([this, &triggerThreadStarted]()
        {
            // Pre-allocated respones.
            static const size_t acceptReponseSize = 20;
            static unsigned char acceptReponse[acceptReponseSize] =
            {
                Core::Packet::ConnectionType, 0, 0, Core::Packet::ConnectionTypeAccept,
                0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0
            };

            // Thread is running.
            triggerThreadStarted.NotifyOne();

            while(m_Stopping == false)
            {
                m_TriggerThreadSemaphore.Wait();

                if(m_Stopping)
                {
                    return;
                }

                // Get next trigger.
                Core::Trigger * pTrigger = nullptr;
                {
                    Core::SafeGuard sf(m_TriggerQueue);
                    if(m_TriggerQueue.Value.size() == 0)
                    {
                        continue;
                    }

                    pTrigger = m_TriggerQueue.Value.front();
                    m_TriggerQueue.Value.pop();
                }

                // Handle current trigger.
                switch(pTrigger->type)
                {
                    case Core::Trigger::OnPeerPreConnect:
                    {
                        Core::OnPeerPreConnectTrigger * pCastTrigger = static_cast<Core::OnPeerPreConnectTrigger *>(pTrigger);

                        // Call trigger
                        bool acceptPeer = false;
                        if(m_OnPeerPreConnect)
                        {
                            acceptPeer = m_OnPeerPreConnect(pCastTrigger->peer);
                        }
                        else
                        {
                            acceptPeer = OnPeerPreConnect(pCastTrigger->peer);
                        }

                        if(acceptPeer)
                        {
                            pCastTrigger->peer->m_State = Core::PeerImp::Accepted;

                            // Send accpet packet.
                            const unsigned long long receiveTime = REALNET_ENDIAN_64(pCastTrigger->receiveTime.AsMicroseconds());
                            memcpy(acceptReponse + 4, &receiveTime, 8);

                            pCastTrigger->peer->m_Clock.Mutex.lock();
                            pCastTrigger->peer->m_Clock.Value.Start();
                            const Time & acceptTime = pCastTrigger->peer->m_Clock.Value.StartTime();
                            pCastTrigger->peer->m_Clock.Mutex.unlock();

                            const unsigned long long sendTime = REALNET_ENDIAN_64(acceptTime.AsMicroseconds());
                            memcpy(acceptReponse + 12, &sendTime, 8);

                            if(m_Socket.Send(acceptReponse, acceptReponseSize, pCastTrigger->peer->m_SocketAddress) != acceptReponseSize)
                            {
                                InternalDisconnectPeer(pCastTrigger->peer, true, false);
                                std::cout << "Failed to send accept message." << std::endl;
                            }

                            Core::SafeGuard sf_peers(m_PeersMutex);
                            m_HandshakingPeers.insert(pCastTrigger->peer);
                            m_ConnectionThreadSemaphore.NotifyOne();

                        }
                        else
                        {
                            InternalDisconnectPeer(pCastTrigger->peer, false, true);
                        }

                    }
                    break;
                    case Core::Trigger::OnPeerConnect:
                    {
                        Core::OnPeerConnectTrigger * pCastTrigger = static_cast<Core::OnPeerConnectTrigger *>(pTrigger);

                        if(m_OnPeerConnect)
                        {
                            m_OnPeerConnect(pCastTrigger->peer);
                        }
                        else
                        {
                            OnPeerConnect(pCastTrigger->peer);
                        }
                    }
                    break;
                    case Core::Trigger::OnPeerDisconnect:
                    {
                        Core::OnPeerDisconnectTrigger * pCastTrigger = static_cast<Core::OnPeerDisconnectTrigger *>(pTrigger);

                        if(m_OnPeerDisconnect)
                        {
                            m_OnPeerDisconnect(pCastTrigger->peer);
                        }
                        else
                        {
                            OnPeerDisconnect(pCastTrigger->peer);
                        }
                    }
                    break;
                    default:
                        break;
                }

                delete pTrigger;
            }
        });


        // Wait for all threads to start running.
        receiveThreadStarted.Wait();
        reliableThreadStarted.Wait();
        connectionThreadStarted.Wait();
        triggerThreadStarted.Wait();

        m_Hosted = true;
    }

    void Server::Stop()
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_Hosted == false)
        {
            return;
        }

        m_Hosted = false;
        m_Stopping = true;

        // Stop all threads
        m_SocketSelector.Stop();
        m_ReceiveThread.join();

        m_ReliableThread.join();

        m_ConnectionThreadSemaphore.NotifyOne();
        m_ConnectionThread.join();

        m_TriggerThreadSemaphore.NotifyOne();
        m_TriggerThread.join();

        // Disconnect peers
        for(auto it = m_Peers.begin(); it != m_Peers.end(); it++)
        {
            auto peer = it->second;

            if(peer->m_State == Core::PeerImp::Connected)
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
            else if(peer->m_State == Core::PeerImp::Accepted)
            {
                const unsigned char rejectData[5] =
                {
                    Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeClosed
                };

                if(m_Socket.Send(rejectData, 5, peer->m_SocketAddress) != 5)
                {
                    std::cout << "Failed to send reject message." << std::endl;
                }
            }
        }
        m_PeerIds.clear();
        m_Peers.clear();

        // Clear trigger queue
        while(m_TriggerQueue.Value.size())
        {
            m_TriggerQueue.Value.pop();
        }

        // Return all packets and restore packet pool size.
        while(m_ConnectionPacketQueue.Value.size())
        {
            Core::Packet * pPacket = m_ConnectionPacketQueue.Value.front();
            m_ConnectionPacketQueue.Value.pop();
            m_PacketPool.Return(pPacket);
        }


        // Destroy socket and settings.
        m_Socket.Close();
        m_Settings = Settings();
        m_Stopping = false;
    }

    void Server::SetOnPeerPreConnect(const std::function<bool(std::shared_ptr<Net::Peer> peer)> & function)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_OnPeerPreConnect != nullptr)
        {
            throw Exception("Cannot call SetOnPeerPreConnect twice.");
        }

        if(m_Hosted)
        {
            throw Exception("Cannot call SetOnPeerPreConnect while hosted.");
        }

        m_OnPeerPreConnect = function;
    }

    void Server::SetOnPeerConnect(const std::function<void(std::shared_ptr<Net::Peer> peer)> & function)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_OnPeerConnect != nullptr)
        {
            throw Exception("Cannot call SetOnPeerConnect twice.");
        }

        if(m_Hosted)
        {
            throw Exception("Cannot call SetOnPeerConnect while hosted.");
        }

        m_OnPeerConnect = function;
    }

    void Server::SetOnPeerDisconnect(const std::function<void(std::shared_ptr<Net::Peer> peer)> & function)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_OnPeerDisconnect != nullptr)
        {
            throw Exception("Cannot call SetOnPeerDisconnect twice.");
        }

        if(m_Hosted)
        {
            throw Exception("Cannot call SetOnPeerDisconnect while hosted.");
        }

        m_OnPeerDisconnect = function;
    }

    const Settings & Server::GetSettings() const
    {
        return m_Settings;
    }

    bool Server::OnPeerPreConnect(std::shared_ptr<Net::Peer> peer)
    {
        return true;
    }

    void Server::OnPeerConnect(std::shared_ptr<Net::Peer> peer)
    {
    }

    void Server::OnPeerDisconnect(std::shared_ptr<Net::Peer> peer)
    {
    }

}
