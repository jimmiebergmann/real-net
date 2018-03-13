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

#include <Time.hpp>
#include <set>
#include <mutex>

namespace Net
{

    namespace Core
    {

        /**
        * @breif Pool of packets, containing packets.
        *
        */
        class PacketPool
        {

        public:

            /**
            * @breif Packet class, containing packet data, allocated by the pool.
            *
            */
            class Packet
            {

            public:

                friend class PacketPool;

                void SetReceiveTime(const Time & time);

                void SetSequence(const unsigned short sequence);

                void SizeSize(const size_t packetSize);

                unsigned char * GetData();

                size_t GetSize() const;

                unsigned short GetSequence() const;

                const Time & GetReceiveTime() const;

            private:

                Packet(const Packet & packet);

                Packet(const size_t dataSize);

                ~Packet();

                unsigned char * m_pData;
                size_t          m_PacketSize;
                unsigned short  m_Sequence;
                Time            m_ReceiveTime;
            };

            PacketPool( const size_t packetSize,
                        const size_t poolSize);

            ~PacketPool();

            size_t IncreasePoolSize(const size_t count);

            size_t DecreasePoolSize(const size_t count);

            size_t GetPacketSize() const;

            size_t GetPoolSize();

            Packet * Get();

            void Return(Packet * packet);

        private:

            PacketPool(const PacketPool & packetPool);

            inline Packet * AddPacket(const bool isFree = true)
            {
                Packet * pPacket = new Packet(m_PacketSize);
                m_AllPackets.insert(pPacket);
                if(isFree)
                {
                    m_FreePackets.insert(pPacket);
                }
                return pPacket;
            }

            const size_t            m_PacketSize;       ///< Size of each packet.
            size_t                  m_PoolSize;         ///< Number of packets, pre-allocated in pool.
            std::set<Packet *>      m_AllPackets;        ///< Set of alla packets.
            std::set<Packet *>      m_FreePackets;      ///< Set of alla packets.
            std::mutex              m_Mutex;            ///< Mutex for packet creation.
        };

    }
}

