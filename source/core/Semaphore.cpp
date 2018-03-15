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

#include <core/Semaphore.hpp>

namespace Net
{

    namespace Core
    {

        Semaphore::Semaphore() :
            m_Count(0)
        {
        }

        void Semaphore::NotifyOne()
        {
            std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
            ++m_Count;
            m_Condition.notify_one();
        }

        void Semaphore::NotifyAll()
        {
            std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
            m_Count = 0;
            m_Condition.notify_all();
        }

        void Semaphore::Wait()
        {
            std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
            while (!m_Count)
            {
                m_Condition.wait(lock);
            }
            --m_Count;
        }

        bool Semaphore::TryWait()
        {
            std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
            if (m_Count)
            {
                --m_Count;
                return true;
            }

            return false;
        }

        bool Semaphore::WaitFor(const Time & timeout)
        {
            std::unique_lock<decltype(m_Mutex)> lock(m_Mutex);
            if (!m_Count)
            {
                // Ugly hack for inifinity wait time.
                if(timeout == Time::Infinite || timeout.AsMicroseconds() > 9223372036854775800ULL)
                {
                    m_Condition.wait(lock);
                    --m_Count;
                    return true;
                }

                if (m_Condition.wait_for(lock, std::chrono::microseconds(timeout.AsMicroseconds())) == std::cv_status::no_timeout)
                {
                    --m_Count;
                    return true;
                }
            }

            return false;
        }

    }

}
