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

#pragma once

#include <core/SocketSelector.hpp>
#include <core/UdpSocket.hpp>
#include <core/Semaphore.hpp>
#include <core/Safe.hpp>
#include <core/PacketPool.hpp>
#include <core/Trigger.hpp>
#include <Peer.hpp>
#include <map>
#include <queue>
#include <set>
#include <thread>
#include <atomic>

namespace Net
{

    namespace Core
    {

        class ServerImp
        {

        protected:

            /**
            * @breif Constructor.
            *
            */
            ServerImp();

            /**
            * @breif Add packet to connection queue.
            *        Packets in the connection queue will be handled by the connection thread.
            *
            */
            void QueueConnectionPacket(Packet * packet);

            /**
            * @breif Add trigger to queue.
            *
            * @throw Exception if trigger is nullptr.
            *
            */
            void AddTrigger(Trigger * trigger);

            std::atomic<bool>                       m_Hosted;                    ///< Is the server currently hosted?
            std::atomic<bool>                       m_Stopping;                  ///< Is the server currently stopping?
            std::thread                             m_ReceiveThread;             ///< Thread for receiving packets.
            std::thread                             m_ReliableThread;            ///< Thread for handling reliable packets.

            Safe<std::map<SocketAddress, Peer *>>   m_ConnectedPeers;            ///< Map of all connected peers.
            Safe<std::map<SocketAddress, Peer *>>   m_EstablishingPeers;         ///< Map of all peers currently establishing a connection.
            Safe<std::map<Time, SocketAddress>>     m_EstablishingTimeouts;      ///< Map of peers establishing, timeout clock as key.
            Safe<std::set<Peer *>>                  m_PeerCleanup;               ///< Set of peers to clean up.


            SocketSelector                          m_SocketSelector;            ///< Socket selector for receive socket.
            UdpSocket                               m_Socket;                    ///< Socket for receiving and sending data.
            PacketPool                              m_PacketPool;                ///< Pool of packets;
            std::mutex                              m_HostStopMutex;             ///< Mutex for hosting and stopping server.

            std::thread                             m_ConnectionThread;          ///< Thread for handling incoming and established connections.
            Safe<std::queue<Packet *>>              m_ConnectionPacketQueue;     ///< Queue of connection packets.
            Semaphore                               m_ConnectionPacketSemaphore; ///< Sempahore for triggering the connection thread.

            std::thread                             m_TriggerThread;             ///< Thread handling all trigger functions.
            Safe<std::queue<Trigger *>>             m_TriggerQueue;              ///< Queue of triggers.
            Semaphore                               m_TriggerSemaphore;          ///< Sempahore for triggering the trigger thread.
            std::function<bool(Peer & peer)>        m_OnPeerPreConnect;
            std::function<void(Peer & peer)>        m_OnPeerConnect;
            std::function<void(Peer & peer)>        m_OnPeerDisconnect;


            REALNET_TEST_FRIEND                 ///< Allow private tests.
        };

    }
}
