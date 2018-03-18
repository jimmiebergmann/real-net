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
#include <Settings.hpp>
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
            * @breif Disconnect peer by pointer.
            *
            */
            bool InternalDisconnectPeer(Peer * peer);

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

            /**
            * @breif Get the next free peer ID.
            *
            */
            unsigned int GetNextPeerId();

            Settings                            m_Settings;                  ///< Server settings.
            std::atomic<bool>                   m_Hosted;                    ///< Is the server currently hosted?
            std::atomic<bool>                   m_Stopping;                  ///< Is the server currently stopping?
            std::mutex                          m_HostStopMutex;             ///< Mutex for hosting and stopping server.
            std::thread                         m_ReceiveThread;             ///< Thread for receiving packets.
            std::thread                         m_ReliableThread;            ///< Thread for handling reliable packets.

            std::mutex                          m_PeersMutex;                ///< Mutex lock for m_Peers, m_ConnectedPeers and m_HandshakingPeers.
            std::map<unsigned int, Peer*>       m_IdPeers;                   ///< Map of all peers, id as key.
            std::map<SocketAddress, Peer *>     m_AddressPeers;              ///< Map of all peers. Address as key.
            Safe<std::set<Peer *>>              m_PeerCleanup;               ///< Set of peers to clean up.
            std::atomic<unsigned int>           m_LastPeerId;                ///< Last peer id in use.

            UdpSocket                           m_Socket;                    ///< Socket for receiving and sending data.
            SocketSelector                      m_SocketSelector;            ///< Socket selector for receive socket.
            PacketPool                          m_PacketPool;                ///< Pool of packets

            std::thread                         m_ConnectionThread;          ///< Thread for handling incoming and established connections.
            Semaphore                           m_ConnectionThreadSemaphore; ///< Sempahore for triggering the connection thread.
            Safe<std::queue<Packet *>>          m_ConnectionPacketQueue;     ///< Queue of connection packets.

            std::thread                         m_TriggerThread;             ///< Thread handling all trigger functions.
            Semaphore                           m_TriggerThreadSemaphore;          ///< Sempahore for triggering the trigger thread.
            Safe<std::queue<Trigger *>>         m_TriggerQueue;              ///< Queue of triggers.
            std::function<bool(Peer & peer)>    m_OnPeerPreConnect;
            std::function<void(Peer & peer)>    m_OnPeerConnect;
            std::function<void(Peer & peer)>    m_OnPeerDisconnect;


            REALNET_TEST_FRIEND                 ///< Allow private tests.
        };

    }
}
