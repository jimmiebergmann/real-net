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
        * @breif Get start time.
        *
        */
        Time GetStartTime();

        /**
        * @breif Get stop time.
        *
        */
        Time GetStopTime();

        /**
        * @breif Get lapsed time.
        *
        * @return Lapsed time since Start time if clock is still running(not stopped),
        *         else difference in time between Start and Stop.
        *
        */
        Time GetLapsedTime();

        /**
        * @breif Get system time.
        *
        */
        static Time GetSystemTime();

    private:

        Time m_StartTime;   ///< The start time
        Time m_StopTime;    ///< The stop time.

    };

}
