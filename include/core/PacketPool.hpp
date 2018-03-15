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

#include <core/Packet.hpp>
#include <core/Safe.hpp>
#include <set>

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
            * @breif Constructor.
            *
            * @param poolSize Number of pre-allocated packets in pool.
            *
            */
            PacketPool(const size_t poolSize);

            /**
            * @breif Destructor.
            *
            */
            ~PacketPool();

            /**
            * @breif Increment number of pre-allocated packets.
            *
            */
            size_t IncreasePoolSize(const size_t count);

            /**
            * @breif Decrease number of pre-allocated packets.
            *        Data is deallocated at packet return if needed.
            *
            */
            size_t DecreasePoolSize(const size_t count);

            /**
            * @breif Get number of pre-allocated packets.
            *
            */
            size_t GetPoolSize();

            /**
            * @breif Get free packet count.
            *
            */
            size_t GetFreeCount();

            /**
            * @breif Get total packet count in pool.
            *
            */
            size_t GetTotalCount();

            /**
            * @breif Get packet from pool.
            *
            */
            Packet * Get();

            /**
            * @breif Return packet to pool.
            *
            * @throw Exception          If packet is returned to wrong pool or
            *                           if packet already is free.
            *
            */
            void Return(Packet * packet);

        private:

            /**
            * @breif Copy constructor. Not defined and allowed.
            *
            */
            PacketPool(const PacketPool & packetPool);

            size_t                  m_PoolSize;         ///< Number of packets, pre-allocated in pool.
            std::set<Packet *>      m_AllPackets;        ///< Set of alla packets.
            std::set<Packet *>      m_FreePackets;      ///< Set of alla packets.
            std::mutex              m_Mutex;            ///< Mutex for packet creation.
        };

    }
}

