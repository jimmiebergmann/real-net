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
                        if(peerIt->second->m_State != Core::PeerImp::InternalConnected && packetType != Core::Packet::ConnectionType)
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

                // Get new and fresh packet to populate with data.
                pPacket = m_PacketPool.Get();

                // Yielf thread.
                std::this_thread::yield();
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
                    if(peer && peer->m_State() == Core::PeerImp::InternalDisconnected)
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

                                    Core::SafeGuard sf_peers(m_PeersMutex); ///< Mutex lock.

                                    // Check if peer has been added.
                                    // We need to do this check, because of synchronization between this thread and the packet receiving thread.
                                    auto peerIt = m_Peers.find(pPacket->address);
                                    if(peerIt != m_Peers.end())
                                    {
                                        peer = peerIt->second;
                                    }

                                    if(peer)
                                    {
                                        // Send accpet packet, since it's already been accepted.
                                        if(peer->m_State() == Core::PeerImp::InternalAccepted)
                                        {
                                            const unsigned long long receiveTime = REALNET_ENDIAN_64(pPacket->receiveTime.AsMicroseconds());
                                            memcpy(acceptReponse + 4, &receiveTime, 8);
                                            const Time serverDelayTime = Clock::SystemTime() - pPacket->receiveTime;
                                            const unsigned long long serverDelay = REALNET_ENDIAN_64(serverDelayTime.AsMicroseconds());
                                            memcpy(acceptReponse + 12, &serverDelay, 8);

                                            if(m_Socket.Send(acceptReponse, acceptReponseSize, peer->m_SocketAddress) != acceptReponseSize)
                                            {
                                                std::cout << "Failed to send accept message." << std::endl;
                                                DisconnectPeer(peer, Peer::Disconnect::InternalError);
                                                break;
                                            }
                                        }

                                        // Ignore init packets, if connected.
                                        // Should we send disconnection packet with reason if disconnected?
                                        break;
                                    }
                                    // New connection.
                                    else
                                    {
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
                                            // Insert new peer.
                                            const unsigned int peerId = GetNextPeerId();
                                            peer = std::make_shared<Peer>();
                                            peer->Initialize(this, peerId, pPacket->address, m_Settings.timeout, m_Settings.latencySamples);
                                            m_Peers.insert({pPacket->address, peer});
                                        }

                                        m_TriggerQueue.Push(new Core::OnPeerPreConnectTrigger(peer, pPacket->receiveTime));
                                    }
                                }
                                break;
                                case Core::Packet::ConnectionTypeAccept:
                                {
                                    if(pPacket->size != 20 || !peer || pPacket->sequence != 1)
                                    {
                                        break;
                                    }

                                    if(peer->m_State() == Core::PeerImp::InternalConnected)
                                    {
                                        // Send ack.
                                        ackReponse[1] = 1;
                                        ackReponse[2] = 0;
                                        if(m_Socket.Send(ackReponse, ackReponseSize, peer->m_SocketAddress) != ackReponseSize)
                                        {
                                            std::cout << "Failed to send acknowledgement message." << std::endl;
                                            DisconnectPeer(peer, Peer::Disconnect::InternalError);
                                            break;
                                        }
                                    }
                                    else if(peer->m_State() == Core::PeerImp::InternalAccepted)
                                    {
                                        Core::SafeGuard sf_peerConnection(m_PeerConnectMutex);

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
                                            DisconnectPeer(peer, Peer::Disconnect::InvalidPacket);
                                            break;
                                        }

                                        // Send acception. Mutex protect, in order to check if peer disconnected while executing acception.
                                        {
                                            Core::SafeGuard sf_peers(m_PeerDisconnectMutex);

                                            if(peer->m_State() != Core::PeerImp::InternalAccepted)
                                            {
                                                break;
                                            }

                                            // Send ack.
                                            ackReponse[1] = 1;
                                            ackReponse[2] = 0;
                                            if(m_Socket.Send(ackReponse, ackReponseSize, peer->m_SocketAddress) != ackReponseSize)
                                            {
                                                std::cout << "Failed to send acknowledgement message." << std::endl;
                                                DisconnectPeer(peer, Peer::Disconnect::InternalError);
                                                break;
                                            }

                                            const Time latency = serverDelay - peerDelay;
                                            peer->m_pLatency->Add(latency);
                                            peer->SetState(Core::PeerImp::InternalConnected);

                                            peer->m_Clock.Mutex.lock();
                                            peer->m_Clock.Value.Reset();
                                            peer->m_Clock.Value.Start();
                                            peer->m_Clock.Mutex.unlock();

                                            m_TriggerQueue.Push(new Core::OnPeerConnectTrigger(peer));
                                        }
                                    }
                                }
                                break;
                                case Core::Packet::ConnectionTypeReject:
                                {
                                    if(pPacket->size != 4 || !peer || peer->m_State() != Core::PeerImp::InternalAccepted || pPacket->sequence != 1)
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

                                    DisconnectPeer(peer, Peer::Disconnect::Left, true, false);
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

                            if(peer)
                            {
                                DisconnectPeer(peer, Peer::Disconnect::Left, true, false);
                            }
                        }
                        break;
                        default:
                            break;
                    };

                    m_PacketPool.Return(pPacket);

                }

                // Handle timeouts.
                Time lowestTimeout = Time::Infinite;
                {
                    Core::SafeGuard sf_peers(m_PeersMutex);

                    for(auto it = m_Peers.begin(); it != m_Peers.end();)
                    {
                        auto peer = it->second;

                        const Core::PeerImp::eInternalState state = peer->m_State();

                        if(state == Core::PeerImp::InternalHandshaking || state == Core::PeerImp::InternalDisconnected)
                        {
                            ++it;
                            continue;
                        }

                        peer->m_Clock.Mutex.lock();
                        const Time time = peer->m_Clock.Value.LapsedTime();
                        peer->m_Clock.Mutex.unlock();

                        if(time.IsZero())
                        {
                            ++it;
                            continue;
                        }

                        Time timeLeft = Time::Infinite;
                        switch(state)
                        {
                            case Core::PeerImp::InternalAccepted:
                            {
                                timeLeft = m_Settings.handshakeTimeout - time;
                                if(timeLeft.IsZero())
                                {
                                    it = DisconnectPeerByIterator(it, Peer::Disconnect::Timeout);
                                    continue;
                                }
                            }
                            break;
                            case Core::PeerImp::InternalConnected:
                            {
                                timeLeft = m_Settings.timeout - time;
                                if(timeLeft.IsZero())
                                {
                                    it = DisconnectPeerByIterator(it, Peer::Disconnect::Timeout);
                                    continue;
                                }
                            }
                            break;
                            default:
                                break;
                        }

                        if(timeLeft < lowestTimeout)
                        {
                            lowestTimeout = timeLeft;
                        }

                        ++it;
                    }

                }

                // Set wait time, in order to execute thread even if we're not receiving any connection packets.
                waitTime = lowestTimeout + Milliseconds(1);

                // Yielf thread.
                std::this_thread::yield();
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
                m_TriggerQueue.semaphore.Wait();

                if(m_Stopping)
                {
                    return;
                }

                // Get next trigger.
                Core::Trigger * pTrigger = m_TriggerQueue.Fetch();

                // Handle current trigger.
                switch(pTrigger->type)
                {
                    case Core::Trigger::OnPeerPreConnect:
                    {
                        Core::OnPeerPreConnectTrigger * pCastTrigger = static_cast<Core::OnPeerPreConnectTrigger *>(pTrigger);

                        if(pCastTrigger->peer->m_State() == Core::PeerImp::InternalDisconnected)
                        {
                            break;
                        }

                        // Call trigger
                        bool acceptPeer = true;
                        if(pCastTrigger->peer->m_State() == Core::PeerImp::InternalHandshaking)
                        {
                            if(m_OnPeerPreConnect)
                            {
                                acceptPeer = m_OnPeerPreConnect(pCastTrigger->peer);
                            }
                            else
                            {
                                acceptPeer = OnPeerPreConnect(pCastTrigger->peer);
                            }
                        }

                        if(acceptPeer)
                        {
                            if(pCastTrigger->peer->m_State() == Core::PeerImp::InternalDisconnected)
                            {
                                break;
                            }
                            else if(pCastTrigger->peer->m_State() == Core::PeerImp::InternalHandshaking)
                            {
                                Core::SafeGuard sf_peerConnection(m_PeerConnectMutex);

                                pCastTrigger->peer->m_Clock.Mutex.lock();
                                pCastTrigger->peer->m_Clock.Value.Start();
                                pCastTrigger->peer->m_Clock.Mutex.unlock();

                                pCastTrigger->peer->SetState(Core::PeerImp::InternalAccepted);

                                // Send accpet packet.
                                const unsigned long long receiveTime = REALNET_ENDIAN_64(pCastTrigger->receiveTime.AsMicroseconds());
                                memcpy(acceptReponse + 4, &receiveTime, 8);
                                const Time serverDelayTime = Clock::SystemTime() - pCastTrigger->receiveTime;
                                const unsigned long long serverDelay = REALNET_ENDIAN_64(serverDelayTime.AsMicroseconds());
                                memcpy(acceptReponse + 12, &serverDelay, 8);

                                if(m_Socket.Send(acceptReponse, acceptReponseSize, pCastTrigger->peer->m_SocketAddress) != acceptReponseSize)
                                {
                                    DisconnectPeer(pCastTrigger->peer, Peer::Disconnect::InternalError);
                                    std::cout << "Failed to send accept message." << std::endl;
                                }

                                m_ConnectionThreadSemaphore.NotifyOne();
                            }
                            else
                            {
                                throw Exception("Trying to accept already accepted peer.");
                            }

                        }
                        else
                        {
                            if(pCastTrigger->peer->m_State() != Core::PeerImp::InternalDisconnected)
                            {
                                DisconnectPeer(pCastTrigger->peer, Peer::Disconnect::Kicked, false);
                            }

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

                        if(pCastTrigger->sendResponse)
                        {
                            const unsigned char disconnectData[4] =
                            {
                                Core::Packet::DisconnectionType, 1, 0, static_cast<unsigned char>(pCastTrigger->reason)
                            };

                            if(m_Socket.Send(disconnectData, 4, pCastTrigger->peer->m_SocketAddress) != 4)
                            {
                                std::cout << "Failed to send disconnect message." << std::endl;
                            }
                        }

                        if(pCastTrigger->triggerFunction)
                        {
                            if(m_OnPeerDisconnect)
                            {
                                m_OnPeerDisconnect(pCastTrigger->peer, pCastTrigger->reason);
                            }
                            else
                            {
                                OnPeerDisconnect(pCastTrigger->peer, pCastTrigger->reason);
                            }
                        }
                    }
                    break;
                    default:
                        break;
                }

                // Delete trigger.
                delete pTrigger;

                // Yielf thread.
                std::this_thread::yield();
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

        m_TriggerQueue.semaphore.NotifyOne();
        m_TriggerThread.join();
/*
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
        m_HandshakingPeers.clear();

        // Clear trigger queue
        while(m_TriggerQueue.Value.size())
        {
            Core::Trigger * pTrigger = m_TriggerQueue.Value.front();
            m_TriggerQueue.Value.pop();
            delete pTrigger;
        }

        // Return all packets and restore packet pool size.
        while(m_ConnectionPacketQueue.Value.size())
        {
            Core::Packet * pPacket = m_ConnectionPacketQueue.Value.front();
            m_ConnectionPacketQueue.Value.pop();
            m_PacketPool.Return(pPacket);
        }*/


        // Destroy socket and settings.
        m_Socket.Close();
        m_Settings = Settings();
        m_Stopping = false;
    }

   /*void Server::SetPeerTimeout(std::shared_ptr<Peer> peer, const Time & timeout)
    {
        peer->m_Timeout = timeout.AsMicroseconds();
    }*/

    void Server::SetOnPeerPreConnect(const std::function<bool(std::shared_ptr<Net::Peer> peer)> & function)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_OnPeerPreConnect)
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

        if(m_OnPeerConnect)
        {
            throw Exception("Cannot call SetOnPeerConnect twice.");
        }

        if(m_Hosted)
        {
            throw Exception("Cannot call SetOnPeerConnect while hosted.");
        }

        m_OnPeerConnect = function;
    }

    void Server::SetOnPeerDisconnect(const std::function<void(std::shared_ptr<Net::Peer> peer,
                                                              Peer::Disconnect::eReason reason)> & function)
    {
        Core::SafeGuard sf_HostStop(m_HostStopMutex);

        if(m_OnPeerDisconnect)
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

    void Server::OnPeerDisconnect(std::shared_ptr<Net::Peer> peer, Peer::Disconnect::eReason reason)
    {
    }

}
