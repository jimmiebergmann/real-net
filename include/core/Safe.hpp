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

#include <mutex>

namespace Net
{

    namespace Core
    {

        /**
        * @breif Helper class for thread safe variables.
        *
        */
        template<typename T>
        class Safe
        {

        public:

            /**
            * @breif Default constructor.
            *
            */
            Safe()
            {
            }

            /**
            * @breif Assignment constructor.
            *
            */
            Safe(const T & value) :
                Value(value)
            {
            }

            /**
            * @breif Copy constructor.
            *
            */
            Safe(const Safe & safe) :
                Value(safe.Get())
            {
            }

            /**
            * @breif Destructor.
            *
            */
            ~Safe()
            {
            }

            /**
            * @breif Thread safe function for obtaining value.
            *
            * @return Value.
            *
            */
            const T Get()
            {
                std::lock_guard<std::mutex> gm(Mutex);
                return Value;
            }

            /**
            * @breif Thread safe function for assigning value.
            *
            */
            void Set(const T & value)
            {
                std::lock_guard<std::mutex> gm(Mutex);
                Value = value;
            }

            /**
            * @breif Thread safe operation for obtaining value.
            *
            */
            const T & operator () ()
            {
                std::lock_guard<std::mutex> gm(Mutex);
                return Value;
            }

            /**
            * @breif Thread safe operation for assigning value.
            *
            */
            Safe & operator = (const T & value)
            {
                std::lock_guard<std::mutex> gm(Mutex);
                Value = value;
                return *this;
            }

            /**
            * @breif Thread safe operation for assigning value from another Safe class.
            *
            */
            Safe & operator = (const Safe & safe)
            {
                std::lock_guard<std::mutex> gm(Mutex);
                Value = safe.Get();
                return *this;
            }

            T			Value; ///< Template value.
            std::mutex	Mutex; ///< Mutex locking in thread safe methods.

        };


        /**
        * @breif Helper class for securing blocks of code from deadlocks.
        *
        */
        class SafeGuard
        {

        public:

            /**
            * @breif Constructor.
            *		 Locking std::mutex.
            *
            */
            SafeGuard(std::mutex & mutex) :
                m_Mutex(mutex)
            {
                m_Mutex.lock();
            }

            /**
            * @breif Constructor.
            *		 Locking safe variable.
            *
            */
            template<typename T>
            SafeGuard(Safe<T> & safe) :
                m_Mutex(safe.Mutex)
            {
                m_Mutex.lock();
            }

            /**
            * @breif Destructor.
            *		 Unlocking mutex.
            *
            */
            ~SafeGuard()
            {
                m_Mutex.unlock();
            }

        private:

            std::mutex & m_Mutex; ///< Reference to locked mutex.

        };

    }
}

