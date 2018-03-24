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
#include <core/DataQueue.hpp>
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

        public:

            friend class Net::Peer;

            /**
            * @breif Disconnect peer from server.
            *
            */
            void DisconnectPeer(std::shared_ptr<Peer> peer, const Peer::Disconnect::eReason reason,
                                const bool triggerFunction = true, const bool sendResponse = true);

        protected:

            typedef std::map<SocketAddress, std::shared_ptr<Peer>> PeerMap; ///< Map of peers, address as key.

            /**
            * @breif Constructor.
            *
            */
            ServerImp();

            /**
            * @breif Disconnect peer from server by raw pointer.
            *        This function is primary used by Peer class, allowing them to disconnect themselves.
            *
            * @throw Exception  If raw pointer is unkown to server.
            *
            */
            void DisconnectPeerByPointer(Peer * peer, const Peer::Disconnect::eReason reason);

            /**
            * @breif Disconnect peer from server by iterator.
            *        This function is primary used by timeout checking.
            *        No lock for peer map is used and the function will return iterator from map::erase.
            *
            */
            PeerMap::iterator DisconnectPeerByIterator(PeerMap::iterator it, const Peer::Disconnect::eReason reason);

            /**
            * @breif Get the next free peer ID.
            *
            */
            unsigned int GetNextPeerId();

            Settings                                         m_Settings;                  ///< Server settings.
            std::atomic<bool>                                m_Hosted;                    ///< Is the server currently hosted?
            std::atomic<bool>                                m_Stopping;                  ///< Is the server currently stopping?
            std::mutex                                       m_HostStopMutex;             ///< Mutex for hosting and stopping server.
            std::thread                                      m_ReceiveThread;             ///< Thread for receiving packets.
            std::thread                                      m_ReliableThread;            ///< Thread for handling reliable packets.

            std::mutex                                       m_PeersMutex;                ///< Mutex lock for m_Peers, m_ConnectedPeers and m_HandshakingPeers.
            PeerMap                                          m_Peers;                     ///< Map of all peers. Address as key.
            std::set<unsigned int>                           m_PeerIds;                   ///< Set of all peer IDs in use.
            std::atomic<unsigned int>                        m_LastPeerId;                ///< Last peer id in use.

            UdpSocket                                        m_Socket;                    ///< Socket for receiving and sending data.
            SocketSelector                                   m_SocketSelector;            ///< Socket selector for receive socket.
            PacketPool                                       m_PacketPool;                ///< Pool of packets

            std::mutex                                       m_PeerConnectMutex;          ///< Mutex for trigger and connection thread.
            std::mutex                                       m_PeerDisconnectMutex;       ///< Mutex for locking Disconnect() function.
            std::thread                                      m_ConnectionThread;          ///< Thread for handling incoming and established connections.
            DataQueue<Packet *>                              m_ConnectionPacketQueue;     ///< Queue of connection packets.

            std::thread                                      m_TriggerThread;             ///< Thread handling all trigger functions.
            DataQueue<Trigger *>                             m_TriggerQueue;              ///< Queue of triggers.
            std::function<bool(std::shared_ptr<Peer>)>       m_OnPeerPreConnect;          ///< Replacing virtual function if set.
            std::function<void(std::shared_ptr<Peer>)>       m_OnPeerConnect;             ///< Replacing virtual function if set.
            std::function<void(std::shared_ptr<Peer>,
                               Peer::Disconnect::eReason)>   m_OnPeerDisconnect;          ///< Replacing virtual function if set.


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
