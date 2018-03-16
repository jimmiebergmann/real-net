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

    void Server::Host(const unsigned short port, Address::eType family)
    {
        Core::SafeGuard sf(m_StopMutex);

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

        m_Socket.Open(port, family);
        m_SocketSelector.Start(&m_Socket);

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
                pPacket->size = static_cast<size_t>(receiveSize);
                pPacket->SerializeSequenceNumber();

                switch(pPacket->data[0])
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
            connectionThreadStarted.NotifyOne();

            Time waitTime = Time::Infinite;

            while(m_Stopping == false)
            {
                m_ConnectionPacketSemaphore.WaitFor(waitTime);

                if(m_Stopping)
                {
                    return;
                }

                // Handle connection packets
                Core::Packet * pPacket = nullptr;
                {
                    Core::SafeGuard sf(m_ConnectionPacketQueue);
                    if(m_ConnectionPacketQueue.Value.size())
                    {
                        pPacket = m_ConnectionPacketQueue.Value.front();
                        m_ConnectionPacketQueue.Value.pop();
                    }

                }

                if(pPacket != nullptr)
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

                                    AddTrigger(new Core::OnPeerPreConnectTrigger(pPacket->address, pPacket->receiveTime));

                                    std::cout << "Received " << pPacket->size << " bytes - connection packet." << std::endl;
                                    std::cout << "Sequence " << pPacket->sequence << "." << std::endl;
                                    std::cout << "From " << pPacket->address.Ip.GetAsString() << ":" << pPacket->address.Port << std::endl;
                                    std::cout << "Delay: " << (Clock::GetSystemTime() - pPacket->receiveTime).AsMicroseconds() << std::endl  << std::endl;
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
                            // ...

                            std::cout << "Received " << pPacket->size << " bytes - disconnection packet." << std::endl;
                            std::cout << "Sequence " << pPacket->sequence << "." << std::endl;
                            std::cout << "From " << pPacket->address.Ip.GetAsString() << ":" << pPacket->address.Port << std::endl << std::endl;
                        }
                        break;
                        default:
                            break;
                    };

                    m_PacketPool.Return(pPacket);

                }


                // Handle connection timeouts.
                // ...

            }
        });

        // Trigger queue.
        Core::Semaphore triggerThreadStarted;
        m_TriggerThread = std::thread([this, &triggerThreadStarted]()
        {
            triggerThreadStarted.NotifyOne();

            while(m_Stopping == false)
            {
                m_TriggerSemaphore.Wait();

                if(m_Stopping)
                {
                    return;
                }

                // Handle connection packets
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

                switch(pTrigger->type)
                {
                    case Core::Trigger::OnPeerPreConnect:
                    {
                        Core::OnPeerPreConnectTrigger * pCastTrigger = static_cast<Core::OnPeerPreConnectTrigger *>(pTrigger);

                        Peer peer(0, pCastTrigger->address);

                        if(OnPeerPreConnect(peer))
                        {
                            // Send accpet packet by default...
                            static const size_t acceptDataSize = 20;
                            static unsigned char acceptData[acceptDataSize] =
                            {
                                0, 1, 0, Core::Packet::ConnectionTypeAccept,   0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0
                            };

                            const unsigned long long receiveTime = REALNET_ENDIAN_64(pCastTrigger->receiveTime.AsMicroseconds());
                            memcpy(acceptData + 4, &receiveTime, 8);
                            const unsigned long long sendTime = REALNET_ENDIAN_64(Clock::GetSystemTime().AsMicroseconds());
                            memcpy(acceptData + 12, &sendTime, 8);

                            if(m_Socket.Send(acceptData, acceptDataSize, pCastTrigger->address) != acceptDataSize)
                            {
                                std::cout << "Failed to send accept message." << std::endl;
                                break;
                            }

                            std::cout << "Sent accept packet." << std::endl;
                        }
                        else
                        {

                        }

                    }
                    break;
                    case Core::Trigger::OnPeerConnect:
                    {
                    }
                    break;
                    case Core::Trigger::OnPeerDisconnect:
                    {
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
        Core::SafeGuard sf(m_StopMutex);

        if(m_Hosted == false)
        {
            return;
        }

        m_Hosted = false;
        m_Stopping = true;


        m_SocketSelector.Stop();
        m_ReceiveThread.join();

        m_ReliableThread.join();

        m_ConnectionPacketSemaphore.NotifyOne();
        m_ConnectionThread.join();

        m_TriggerSemaphore.NotifyOne();
        m_TriggerThread.join();

        m_Socket.Close();
        m_Stopping = false;
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
