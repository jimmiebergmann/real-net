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
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace Net
{

    namespace Core
    {

        class Semaphore
        {

        public:

            /**
            * @breif Constructor.
            *
            */
            Semaphore();

            /**
            * @breif Notify one wait.
            *
            */
            void NotifyOne();

            /**
            * @breif Notify all waits.
            *
            */
            void NotifyAll();

            /**
            * @breif Wait for notifies.
            *
            */
            void Wait();

            /**
            * @breif Try to wait.
            *
            * @return False if function call will result in a wait, else true.
            *
            */
            bool TryWait();

            /**
            * @breif Wait until given timeout is reached.
            *
            * @return False if timeout is reached, else false.
            *
            */
            bool WaitFor(const Time & timeout);

        private:

            std::mutex m_Mutex;
            std::condition_variable m_Condition;
            unsigned long m_Count;
        };

    }
}
