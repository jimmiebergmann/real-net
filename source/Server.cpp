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
        Settings tempSettings(port, maxConnections);
        Host(tempSettings);
    }

    void Server::Host(const unsigned short port, const size_t maxConnections, const Settings & settings)
    {
        Settings tempSettings = settings;
        if(port != 0)
        {
            tempSettings.port = port;
        }
        if(maxConnections != 0)
        {
            tempSettings.maxConnections = maxConnections;
        }
        Host(tempSettings);
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

                pPacket->receiveTime = Clock::GetSystemTime();

                const unsigned char packetType = pPacket->data[0];

                // Find peer
                {
                    Core::SafeGuard sf_peers(m_PeersMutex);

                    // Find peer
                    Peer * pPeer = nullptr;
                    auto peerIt = m_AddressPeers.find(pPacket->address);
                    if(peerIt != m_AddressPeers.end())
                    {
                        pPeer = peerIt->second;
                        Core::PeerImp::AttachPacket(pPacket, pPeer);
                    }

                    if(pPeer == nullptr)
                    {
                        // Completely new peers are only allowed to send connection packets.
                        if(packetType != Core::Packet::ConnectionType)
                        {
                            continue;
                        }
                    }
                    else if(pPeer->m_State == Core::PeerImp::Handshaking)
                    {
                        // Handshaking peers are only allowed to send connection and disconnection packets.
                        if(packetType != Core::Packet::ConnectionType && packetType != Core::Packet::DisconnectionType)
                        {
                            continue;
                        }
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
            static const size_t fullReponseSize = 5;
            static unsigned char fullReponse[fullReponseSize] =
            {
                 Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeFull
            };

            static const size_t ackReponseSize = 3;
            static unsigned char ackReponse[ackReponseSize] =
            {
                 Core::Packet::AcknowledgementType, 0, 0
            };

            connectionThreadStarted.NotifyOne();

            Time waitTime = Microseconds(1000000ULL);//Time::Infinite;

            while(m_Stopping == false)
            {
                m_ConnectionPacketSemaphore.WaitFor(waitTime);

                if(m_Stopping)
                {
                    return;
                }

                // Handle connection packets
                Core::Packet * pPacket = nullptr;
                Peer * pPeer = nullptr;
                {
                    Core::SafeGuard sf_peer(m_ConnectionPacketQueue);
                    if(m_ConnectionPacketQueue.Value.size())
                    {
                        pPacket = m_ConnectionPacketQueue.Value.front();
                        m_ConnectionPacketQueue.Value.pop();
                        pPeer = pPacket->peer;

                        // Do not care about disconnecting peers
                        if(pPeer && pPeer->m_State == Core::PeerImp::Disconnecting)
                        {
                            m_PacketPool.Return(pPacket);
                            pPacket = nullptr;
                        }
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

                                    const bool newPeer = pPeer == nullptr;

                                    // Check if server is full
                                    if(pPeer == nullptr)
                                    {
                                        Core::SafeGuard sf_peers(m_PeersMutex);

                                        if(m_IdPeers.size() >= m_Settings.maxConnections)
                                        {
                                            if(m_Socket.Send(fullReponse, fullReponseSize, pPacket->address) != fullReponseSize)
                                            {
                                                std::cout << "Failed to send full message." << std::endl;
                                            }
                                            break;
                                        }

                                        auto addressPeerIt = m_AddressPeers.find(pPacket->address);
                                        if(addressPeerIt == m_AddressPeers.end())
                                        {
                                            const unsigned int peerId = GetNextPeerId();

                                            pPeer = new Peer(peerId, pPacket->address);
                                            m_IdPeers.insert({peerId, pPeer});
                                            m_AddressPeers.insert({pPacket->address, pPeer});
                                        }
                                    }

                                    AddTrigger(new Core::OnPeerPreConnectTrigger(newPeer, pPeer, pPacket->receiveTime));
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

                            // Disconnect peer.
                            InternalDisconnectPeer(pPeer);

                            // Send ack.
                            const unsigned short sequence = REALNET_ENDIAN_16(pPacket->sequence);
                            memcpy(ackReponse + 1, &sequence, 2);
                            if(m_Socket.Send(ackReponse, ackReponseSize, pPacket->address) != ackReponseSize)
                            {
                                std::cout << "Failed to send acknowledgement message." << std::endl;
                            }
                            break;

                            // Add disconnection trigger.
                            if(pPeer)
                            {
                                AddTrigger(new Core::OnPeerDisconnectTrigger(pPeer));
                            }
                        }
                        break;
                        default:
                            break;
                    };

                    m_PacketPool.Return(pPacket);

                }


                // Handle connection timeouts.
                // ...

                // Handle connecton cleanups.
                {
                    Core::SafeGuard sf_peerCleanup(m_PeerCleanup);

                    for(auto it = m_PeerCleanup.Value.begin(); it != m_PeerCleanup.Value.end();)
                    {
                        Peer * pPeer = *it;

                        if(pPeer->m_ActivePackets.Get() == 0)
                        {
                            {
                                Core::SafeGuard sf_peers(m_PeersMutex);

                                auto idPeerIt = m_IdPeers.find(pPeer->m_Id);
                                auto addressPeerIt = m_AddressPeers.find(pPeer->m_SocketAddress);
                                if(idPeerIt != m_IdPeers.end())
                                {
                                    if(idPeerIt->second != pPeer)
                                    {
                                        throw Exception("Trying to cleanup peer by id, with incorrect id.");
                                    }
                                    m_IdPeers.erase(idPeerIt);
                                }
                                if(addressPeerIt != m_AddressPeers.end())
                                {
                                    if(addressPeerIt->second != pPeer)
                                    {
                                        throw Exception("Trying to cleanup peer by address, with incorrect address.");
                                    }
                                    m_AddressPeers.erase(addressPeerIt);
                                }
                            }

                            std::cout << "Cleaned up peer:" << pPeer->GetId() << std::endl;

                            delete pPeer;
                            it = m_PeerCleanup.Value.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }

                    m_PeerCleanup.Value.clear();
                }

            }
        });

        // Trigger queue.
        Core::Semaphore triggerThreadStarted;
        m_TriggerThread = std::thread([this, &triggerThreadStarted]()
        {
            // Pre-allocated connection respones.
            static const size_t acceptReponseSize = 20;
            static unsigned char acceptReponse[acceptReponseSize] =
            {
                Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeAccept,
                0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0
            };

            static const size_t rejectResponseSize = 5;
            static unsigned char rejectResponse[rejectResponseSize] =
            {
                Core::Packet::ConnectionType, 1, 0, Core::Packet::ConnectionTypeReject, Core::Packet::RejectTypeKicked
            };

            // Thread is running.
            triggerThreadStarted.NotifyOne();

            while(m_Stopping == false)
            {
                m_TriggerSemaphore.Wait();

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

                        const bool newPeer = pCastTrigger->newPeer;
                        Peer * pPeer = pCastTrigger->peer;
                        const SocketAddress & socketAddress = pPeer->GetAddress();

                        // Call trigger
                        bool acceptPeer = newPeer == false;
                        if(newPeer)
                        {
                            if(m_OnPeerPreConnect)
                            {
                                acceptPeer = m_OnPeerPreConnect(*pPeer);
                            }
                            else
                            {
                                acceptPeer = OnPeerPreConnect(*pPeer);
                            }
                        }

                        if(acceptPeer)
                        {
                            // Send accpet packet.
                            const unsigned long long receiveTime = REALNET_ENDIAN_64(pCastTrigger->receiveTime.AsMicroseconds());
                            memcpy(acceptReponse + 4, &receiveTime, 8);
                            const unsigned long long sendTime = REALNET_ENDIAN_64(Clock::GetSystemTime().AsMicroseconds());
                            memcpy(acceptReponse + 12, &sendTime, 8);

                            if(m_Socket.Send(acceptReponse, acceptReponseSize, socketAddress) != acceptReponseSize)
                            {
                                InternalDisconnectPeer(pPeer);
                                std::cout << "Failed to send accept message." << std::endl;
                                break;
                            }
                        }
                        else
                        {
                            // Send reject packet.
                            if(m_Socket.Send(rejectResponse, rejectResponseSize, socketAddress) != rejectResponseSize)
                            {
                                std::cout << "Failed to send reject message." << std::endl;
                            }

                            // Disconnect peer.
                            InternalDisconnectPeer(pPeer);
                        }

                    }
                    break;
                    case Core::Trigger::OnPeerConnect:
                    {
                        Core::OnPeerConnectTrigger * pCastTrigger = static_cast<Core::OnPeerConnectTrigger *>(pTrigger);

                        if(m_OnPeerConnect)
                        {
                            m_OnPeerConnect(*pCastTrigger->peer);
                        }
                        else
                        {
                            OnPeerConnect(*pCastTrigger->peer);
                        }
                    }
                    break;
                    case Core::Trigger::OnPeerDisconnect:
                    {
                        Core::OnPeerDisconnectTrigger * pCastTrigger = static_cast<Core::OnPeerDisconnectTrigger *>(pTrigger);

                        if(m_OnPeerDisconnect)
                        {
                            m_OnPeerDisconnect(*pCastTrigger->peer);
                        }
                        else
                        {
                            OnPeerDisconnect(*pCastTrigger->peer);
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

        m_ConnectionPacketSemaphore.NotifyOne();
        m_ConnectionThread.join();

        m_TriggerSemaphore.NotifyOne();
        m_TriggerThread.join();

        // Disconnect peers
        // ..

        // Destroy socket and settings.
        m_Socket.Close();
        m_Settings = Settings();

        // Return all packets and restore packet pool size.
        while(m_ConnectionPacketQueue.Value.size())
        {
            Core::Packet * pPacket = m_ConnectionPacketQueue.Value.front();
            pPacket->peer = nullptr;
            m_PacketPool.Return(pPacket);
        }

        m_Stopping = false;
    }

    bool Server::DisconnectPeer(const unsigned int id)
    {
        Peer * pPeer = nullptr;
        {
            Core::SafeGuard sf_Peers(m_PeersMutex);
            auto it = m_IdPeers.find(id);
            if(it != m_IdPeers.end())
            {
                pPeer = it->second;
            }
            else
            {
                return false;
            }
        }

        return InternalDisconnectPeer(pPeer);
    }

    void Server::SetOnPeerPreConnect(const std::function<bool(Peer & peer)> & function)
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

    void Server::SetOnPeerConnect(const std::function<void(Peer & peer)> & function)
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

    void Server::SetOnPeerDisconnect(const std::function<void(Peer & peer)> & function)
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

    bool Server::OnPeerPreConnect(Peer & peer)
    {
        return true;
    }

    void Server::OnPeerConnect(Peer & peer)
    {
    }

    void Server::OnPeerDisconnect(Peer & peer)
    {
    }

}
