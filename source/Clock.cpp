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

#include <Clock.hpp>


namespace Net
{

    Clock::Clock() :
        m_StartTime(Time::Zero),
        m_StopTime(Time::Zero)
    {
    }

    void Clock::Start()
    {
        m_StartTime = SystemTime();
        m_StopTime = Time::Zero;
    }

    void Clock::Stop()
    {
        if(m_StopTime != Time::Zero)
        {
            m_StopTime = SystemTime();
        }
    }

    const Time & Clock::StartTime() const
    {
        return m_StartTime;
    }

    const Time & Clock::StopTime() const
    {
        return m_StopTime;
    }

    Time Clock::LapsedTime() const
    {
        return m_StopTime != Time::Zero ? m_StopTime : (SystemTime() - m_StartTime);
    }

    Time Clock::SystemTime()
    {
    // Windows implementation.
    #ifdef REALNET_PLATFORM_WINDOWS
        long long counter = 0;
        long long frequency = 0;

        QueryPerformanceCounter( (LARGE_INTEGER*)&counter );
        QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency );
        double time = (static_cast<double>(counter) * 1000000.0f ) / static_cast<double>(frequency);

        return Microseconds(static_cast<unsigned long long>(timeFloat));

    // Linux implementation.
    #elif defined( REALNET_PLATFORM_LINUX )

        timeval time;
        gettimeofday( &time, 0 );

        return	Microseconds(static_cast<unsigned long long>( time.tv_sec ) * 1000000ULL +
                             static_cast<unsigned long long>( time.tv_usec ));

    #endif
    }

}
