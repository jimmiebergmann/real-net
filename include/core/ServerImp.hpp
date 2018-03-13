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
#include <core/Safe.hpp>
#include <core/PacketPool.hpp>
#include <Peer.hpp>
#include <map>
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

            std::atomic<bool>                   m_Hosted;               ///< Is the server currently hosted?
            std::atomic<bool>                   m_Stopping;             ///< Is the server currently stopping?
            std::map<unsigned short, Peer *>    m_PeerMap;              ///< Map of all connected peers.
            std::thread                         m_ReceiveThread;        ///< Thread for receiving packets.
            std::thread                         m_ReliableThread;       ///< Thread for handling reliable packets.
            std::thread                         m_ConnectionThread;     ///< Thread for handling incoming and established connections.
            SocketSelector                      m_SocketSelector;       ///< Socket selector for receive socket.
            UdpSocket                           m_ReceiveSocket;        ///< Socket for receiving data.
            PacketPool                          m_PacketPool;           ///< Pool of packets;
            const size_t                        m_MaxPacketSize;        ///< Maximum size of each packet.
            std::mutex                          m_StopMutex;            ///< Mutex for starting server.

            REALNET_TEST_FRIEND                 ///< Allow private tests.
        };

    }
}
