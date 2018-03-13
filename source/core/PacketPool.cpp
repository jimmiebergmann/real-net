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

#include <core/PacketPool.hpp>
#include <core/Safe.hpp>
#include <Exception.hpp>

namespace Net
{

    namespace Core
    {

        /// Packet class.
        void PacketPool::Packet::SetReceiveTime(const Time & time)
        {
            m_ReceiveTime = time;
        }

        void PacketPool::Packet::SetSequence(const unsigned short sequence)
        {
            m_Sequence = sequence;
        }

        void PacketPool::Packet::SizeSize(const size_t packetSize)
        {
            m_PacketSize = packetSize;
        }

        unsigned char * PacketPool::Packet::GetData()
        {
            return m_pData;
        }

        size_t PacketPool::Packet::GetSize() const
        {
            return m_PacketSize;
        }

        unsigned short PacketPool::Packet::GetSequence() const
        {
            return m_Sequence;
        }

        const Time & PacketPool::Packet::GetReceiveTime() const
        {
            return m_ReceiveTime;
        }

        PacketPool::Packet::Packet(const Packet & packet)
        {
        }

        PacketPool::Packet::Packet(const size_t dataSize) :
            m_pData(new unsigned char[dataSize]),
            m_PacketSize(0),
            m_Sequence(0)
        {
        }

        PacketPool::Packet::~Packet()
        {
            delete [] m_pData;
        }


        // Packet pool class.
        PacketPool::PacketPool( const size_t packetSize,
                                const size_t poolSize) :
            m_PacketSize(packetSize),
            m_PoolSize(poolSize)
        {
            for(size_t i = 0; i < m_PoolSize; i++)
            {
                AddPacket();
            }
        }

        PacketPool::~PacketPool()
        {
            Core::SafeGuard sf(m_Mutex);

            for(auto it = m_AllPackets.begin(); it != m_AllPackets.end(); it++)
            {
                Packet * pPacket = *it;
                delete pPacket;
            }
        }

        size_t PacketPool::IncreasePoolSize(const size_t count)
        {
            Core::SafeGuard sf(m_Mutex);

            m_PoolSize += count;
            for(size_t i = 0; i < count; i++)
            {
                AddPacket();
            }

            return m_PoolSize;
        }

        size_t PacketPool::DecreasePoolSize(const size_t count)
        {
            Core::SafeGuard sf(m_Mutex);

            if(count < m_PoolSize)
            {
                m_PoolSize -= count;
            }
            else
            {
                m_PoolSize = 0;
            }

            return m_PoolSize;
        }

        size_t PacketPool::GetPacketSize() const
        {
            return m_PacketSize;
        }

        size_t PacketPool::GetPoolSize()
        {
            Core::SafeGuard sf(m_Mutex);
            return m_PoolSize;
        }

        PacketPool::Packet * PacketPool::Get()
        {
            Core::SafeGuard sf(m_Mutex);

            Packet * pPacket = nullptr;
            if(m_FreePackets.size() == 0)
            {
                pPacket = AddPacket(false);
            }
            else
            {
                auto it = m_FreePackets.begin();
                pPacket = *it;
                m_FreePackets.erase(it);
            }

            return pPacket;
        }

        void PacketPool::Return(Packet * packet)
        {
            if(packet == nullptr)
            {
                return;
            }

            Core::SafeGuard sf(m_Mutex);

            auto itAll = m_AllPackets.find(packet);
            if(itAll == m_AllPackets.end())
            {
                throw Exception("Returning packet to wrong pool.");
            }

            auto itFree = m_FreePackets.find(packet);
            if(itFree != m_FreePackets.end())
            {
                throw Exception("Packet is already returned to pool.");
            }


            if(m_AllPackets.size() > m_PoolSize)
            {
                m_AllPackets.erase(itAll);
                delete packet;
                return;
            }

            m_FreePackets.insert(packet);
        }

    }

}
