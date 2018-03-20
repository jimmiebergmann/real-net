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
            * @breif Internal function for disconnecting peer.
            *        Send disconnection packet if currently connected.
            *        Triggers OnPeerDisconnect function.
            *
            */
            void InternalDisconnectPeer(std::shared_ptr<Peer> peer, const bool triggerFunction, const bool sendResponse);

            /**
            * @breif Add packet to connection queue.
            *        Packets in the connection queue will be handled by the connection thread.
            *
            */
            void QueueConnectionPacket(Packet * packet);

            /**
            * @breif Get connect packet from queue.
            *
            * @return Pointer to packet, null if queue is empty.
            *
            */
            Packet * GetConnectionPacket();

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

            Settings                                        m_Settings;                  ///< Server settings.
            std::atomic<bool>                               m_Hosted;                    ///< Is the server currently hosted?
            std::atomic<bool>                               m_Stopping;                  ///< Is the server currently stopping?
            std::mutex                                      m_HostStopMutex;             ///< Mutex for hosting and stopping server.
            std::thread                                     m_ReceiveThread;             ///< Thread for receiving packets.
            std::thread                                     m_ReliableThread;            ///< Thread for handling reliable packets.

            std::mutex                                      m_PeersMutex;                ///< Mutex lock for m_Peers, m_ConnectedPeers and m_HandshakingPeers.
            std::map<SocketAddress, std::shared_ptr<Peer>>  m_Peers;                     ///< Map of all peers. Address as key.
            std::set<unsigned int>                          m_PeerIds;                   ///< Set of all peer IDs in use.
            std::atomic<unsigned int>                       m_LastPeerId;                ///< Last peer id in use.
            std::set<std::shared_ptr<Peer>>                 m_HandshakingPeers;          ///< Set of all handshaking peers.

            UdpSocket                                       m_Socket;                    ///< Socket for receiving and sending data.
            SocketSelector                                  m_SocketSelector;            ///< Socket selector for receive socket.
            PacketPool                                      m_PacketPool;                ///< Pool of packets

            std::thread                                     m_ConnectionThread;          ///< Thread for handling incoming and established connections.
            Semaphore                                       m_ConnectionThreadSemaphore; ///< Sempahore for triggering the connection thread.
            Safe<std::queue<Packet *>>                      m_ConnectionPacketQueue;     ///< Queue of connection packets.

            std::thread                                     m_TriggerThread;             ///< Thread handling all trigger functions.
            Semaphore                                       m_TriggerThreadSemaphore;    ///< Sempahore for triggering the trigger thread.
            Safe<std::queue<Trigger *>>                     m_TriggerQueue;              ///< Queue of triggers.
            std::function<bool(std::shared_ptr<Peer>)>      m_OnPeerPreConnect;          ///< Replacing virtual function if set.
            std::function<void(std::shared_ptr<Peer>)>      m_OnPeerConnect;             ///< Replacing virtual function if set.
            std::function<void(std::shared_ptr<Peer>)>      m_OnPeerDisconnect;          ///< Replacing virtual function if set.


        private:

            /**
            * @breif Deleted copy constructor.
            *
            */
            ServerImp(const ServerImp & serverImp);


            REALNET_TEST_FRIEND                 ///< Allow private tests.
        };

    }
}
