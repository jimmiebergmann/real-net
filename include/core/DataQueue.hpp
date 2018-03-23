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

#include <core/Safe.hpp>
#include <core/Semaphore.hpp>
#include <functional>
#include <queue>

namespace Net
{

    namespace Core
    {

        /**
        * @breif Data queue, containg data of specified template class.
        *
        */
        template <typename Item>
        class DataQueue
        {

        public:

            /**
            * @breif Constructor.
            *
            * @param noFetch    Value returned by Fetch(), if no data is available in queue.
            * @param notify     Specify if semaphore should be notified after item has been pushed.
            *
            */
            DataQueue(Item noFetch, bool notify = false);

            /**
            * @breif Destructor.
            *
            */
            ~DataQueue();

            /**
            * @breif Push data to queue.
            *
            */
            void Push(Item item);

            /**
            * @breif Fetch data from queue.
            *
            */
            Item Fetch();

            /**
            * @breif Clear queue.
            *        Will run function for each data node in queue, if function is specified.
            *
            */
            void Clear();
            void Clear(const std::function<void(Item item)> clearFunction);

            /**
            * @breif Public semaphore.
            *        Will be notified if notify flag is true.
            *
            */
            Semaphore semaphore;

        private:

            std::mutex          m_Mutex;        ///< Mutex, protecting data queue.
            Item                m_DefaultItem;  ///< Value returned by Fetch(), if no data is available in queue.
            std::queue<Item>    m_Queue;        ///< Queue of data.
            const bool          m_Notify;       ///< Sempahore notify flag.

        };

        #include <core/inline/DataQueue.inl> ///< Include inline definitions.

    }
}
