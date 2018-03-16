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

namespace Net
{

    namespace Core
    {

        PeerImp::PeerImp(const unsigned short id, const SocketAddress & socketAddress) :
            m_Id(id),
            m_SocketAddress(socketAddress),
            m_ActivePackets(0),
            m_Disconnected(false)
        {
        }

        void PeerImp::AssignPacket(Packet * packet)
        {
            if(packet == nullptr || reinterpret_cast<PeerImp*>(packet->peer) != this)
            {
                throw Exception("Cannot assign packet to peer. Packet == nullptr, or packet's peer is incorrect.");
            }

            m_ActivePackets++;
        }


        void PeerImp::ReturnPacket(Packet * packet)
        {
            if(packet == nullptr || reinterpret_cast<PeerImp*>(packet->peer) != this)
            {
                throw Exception("Cannot return packet to peer. Packet == nullptr, or packet's peer is incorrect.");
            }

            if(m_ActivePackets == 0)
            {
                throw Exception("Cannot return packet to peer if m_ActivePackets == 0.");
            }

            m_ActivePackets--;
        }
    }
}
