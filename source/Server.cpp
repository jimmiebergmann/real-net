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
#include <core/Semaphore.hpp>
#include <core/Packet.hpp>
#include <Clock.hpp>

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

        m_ReceiveSocket.Open(port, family);
        m_SocketSelector.Start(&m_ReceiveSocket);

        // Receive thead
        Core::Semaphore receiveThreadStarted;
        m_ReceiveThread = std::thread([this, &receiveThreadStarted]()
        {
            SocketAddress socketAddress;
            Core::PacketPool::Packet * pPacket = m_PacketPool.Get();

            receiveThreadStarted.NotifyOne();

            while(m_Stopping == false)
            {
                if(m_SocketSelector.Select(Milliseconds(500ULL)) == false)
                {
                    continue;
                }

                int receiveSize = m_ReceiveSocket.Receive(pPacket->GetData(), m_MaxPacketSize, socketAddress);
                if(receiveSize == 0)
                {
                    continue;
                }

                pPacket->SetReceiveTime(Clock::GetSystemTime());

                if(receiveSize < 0)
                {
                    throw Exception("Receive thread: Error while receiving data: " + std::to_string(Core::GetLastSystemError()));
                    break;
                }

                const size_t packetType = static_cast<size_t>(pPacket->GetData()[0]);
                if(packetType >= Core::Packet::TypeCount)
                {
                    continue;
                }

                switch(packetType)
                {
                    case Core::Packet::ConnectionType:
                        break;
                    case Core::Packet::DisconnectionType:
                        break;
                    case Core::Packet::SynchronizationType:
                        break;
                    case Core::Packet::AcknowledgementType:
                        break;
                    default:
                        break;
                };

            }
        });

        // Reliable thread.
        Core::Semaphore reliableThreadStarted;
        m_ReliableThread = std::thread([this, &reliableThreadStarted]()
        {
            reliableThreadStarted.NotifyOne();

        });

        // Reliable thread.
        Core::Semaphore connectionThreadStarted;
        m_ConnectionThread = std::thread([this, &connectionThreadStarted]()
        {
            connectionThreadStarted.NotifyOne();

        });


        // Wait for all threads to start running.
        receiveThreadStarted.Wait();
        reliableThreadStarted.Wait();
        connectionThreadStarted.Wait();

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

        m_ConnectionThread.join();


        m_ReceiveSocket.Close();
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
