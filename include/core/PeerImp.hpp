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
#include <core/Latency.hpp>
#include <Clock.hpp>
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
        class PeerTrigger;

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
            friend class PeerTrigger;
            friend class OnPeerDisconnectTrigger;

            /**
            * @breif Enumerator of current status of peer.
            *
            */

            enum eInternalState
            {
                InternalHandshaking,
                InternalAccepted,
                InternalConnected,
                InternalDisconnected
            };

            /**
            * @breif Constructor.
            *
            */
            PeerImp();

            /**
            * @breif Destructor.
            *
            */
            ~PeerImp();

            /**
            * @breif Initialize peer class values.
            *
            */
            void Initialize(Server * pServer, const unsigned short id, const SocketAddress & socketAddress,
                            const Time & timeout, const size_t latencySamples);

            /**
            * @breif Set internal state of peer.
            *
            */
            void SetState(const eInternalState state);


            Server *                        m_pServer;          ///< Pointer to server.
            mutable Safe<eInternalState>    m_State;            ///< Current state of peer.
            unsigned short                  m_Id;               ///< Id of peer.
            SocketAddress                   m_SocketAddress;    ///< Ip and port of peer.
            std::atomic<unsigned long long> m_Timeout;          ///< Timeout of peer. Disconnected if not receving any valid synchronization packets for a while.
            mutable Safe<Clock>             m_Clock;            ///< Timer starting when accepted or connected, depending on state.
            Core::Latency *                 m_pLatency;         ///< Latency calculator.

        private:

            /**
            * @breif Deleted copy constructor.
            *
            */
            PeerImp(const PeerImp & peerImp);
        };

    }

}
