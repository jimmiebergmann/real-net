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

namespace Net
{

    class Clock
    {

    public:

        /**
        * @breif State enumerator
        *
        */
        enum eState
        {
            Stopped,
            Started
        };

        /**
        * @breif Constructor.
        *
        */
        Clock();

        /**
        * @breif Start clock.
        *
        */
        void Start();

        /**
        * @breif Stop clock.
        *
        */
        void Stop();

        /**
        * @breif Reset clock.
        *
        */
        void Reset();

        /**
        * @breif Get start time.
        *
        */
        const Time & StartTime() const;

        /**
        * @breif Get stop time.
        *
        */
        const Time & StopTime() const;

        /**
        * @breif Get total paused time.
        *
        */
        const Time PausedTime() const;

        /**
        * @breif Get lapsed time.
        *
        * @return Lapsed time since Start time if clock is still running(not stopped),
        *         else difference in time between Start and Stop.
        *
        */
        Time LapsedTime() const;

        /**
        * @breif Get current state.
        *
        */
        eState State() const;

        /**
        * @breif Get system time.
        *
        */
        static Time SystemTime();

    private:

        eState  m_State;        ///< Current state of clock.
        Time    m_StartTime;    ///< The start time
        Time    m_StopTime;     ///< The stop time.
        Time    m_PausedTime;   ///< Amount of paused time.

    };

}
