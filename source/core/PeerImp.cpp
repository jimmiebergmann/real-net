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

#include <core/PeerImp.hpp>
#include <Peer.hpp>

namespace Net
{

    namespace Core
    {

        PeerImp::PeerImp(const unsigned short id, const SocketAddress & socketAddress, const size_t latencySamples) :
            m_Id(id),
            m_SocketAddress(socketAddress),
            m_ActivePackets(0),
            m_State(eState::Handshaking),
            m_Latency(latencySamples)
        {
        }

        void PeerImp::AddCurrentLatency(const Time & latency, const size_t maxCount)
        {
        }

        void PeerImp::AttachPacket(Packet * packet, Peer * peer)
        {
            if(packet->peer != nullptr)
            {
                throw Exception("Cannot assign packet to peer. Packet is already assigned.");
            }

            packet->peer = peer;

            Core::SafeGuard sf(peer->m_ActivePackets);

            peer->m_ActivePackets.Value++;
        }


        void PeerImp::DetachPacket(Packet * packet)
        {
            Peer * pPeer = nullptr;
            if((pPeer = packet->peer) == nullptr)
            {
                throw Exception("Cannot return packet to peer. Packet is not yet attached.");
            }

            Core::SafeGuard sf(pPeer->m_ActivePackets);

            if(pPeer->m_ActivePackets.Value == 0)
            {
                throw Exception("Cannot return packet to peer if m_ActivePackets == 0.");
            }

            pPeer->m_ActivePackets.Value--;
        }

    }
}
