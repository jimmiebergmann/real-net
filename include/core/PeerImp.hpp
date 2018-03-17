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

#include <core/Build.hpp>
#include <core/Safe.hpp>
#include <core/Packet.hpp>
#include <Address.hpp>
#include <atomic>

namespace Net
{

    class Server; ///< Forward declaration.

    namespace Core
    {

        /**
        * @breif Forward declarations.
        *
        */
        class ServerImp;
        class PacketPool;

        /**
        * @breif Peer implementation class.
        *
        */
        class PeerImp
        {

        protected:

            /**
            * @breif Friend classes.
            *
            */
            friend class Net::Server;
            friend class ServerImp;
            friend class PacketPool;

            enum eState
            {
                Handshaking,
                Connected,
                Disconnecting
            };

            /**
            * @breif Increment active packet counter.
            *        The packet is passed, but not stored, only for error checking.
            *
            * @throw Exception  If passed packet's peer pointer is incorrect.
            *
            */
            static void AttachPacket(Packet * packet, Peer * peer);

            /**
            * @breif Decrement active packet counter.
            *        The packet is passed, but not stored, only for error checking.
            *
            * @throw Exception  If passed packet's peer pointer is incorrect.
            *
            */
            static void DetachPacket(Packet * packet);

            /**
            * @breif Constructor.
            *
            */
            PeerImp(const unsigned short id, const SocketAddress & socketAddress);

            unsigned short      m_Id;               ///< Id of peer.
            SocketAddress       m_SocketAddress;    ///< Ip and port of peer.

            Safe<size_t>        m_ActivePackets;    ///< Peer packets in use. Cannot be deallocated if not equal to 0.
            std::atomic<eState> m_State;            ///< Current state of peer.
        };

    }

}
