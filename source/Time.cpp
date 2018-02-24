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

#include <Time.hpp>

namespace Net
{
    const Time Time::Infinite = Microseconds(0xFFFFFFFFFFFFFFFFULL);
    const Time Time::Zero = Microseconds(0);


    Time::Time() :
        m_Microseconds(0)
    {
    }

    double Time::AsSeconds() const
    {
        return static_cast<double>(m_Microseconds) / 1000000.0f;
    }

    unsigned long long Time::AsMilliseconds() const
    {
        return m_Microseconds / 1000ULL;
    }

    unsigned long long Time::AsMicroseconds() const
    {
        return m_Microseconds;
    }

    bool Time::operator == (const Time & time) const
    {
        return m_Microseconds == time.m_Microseconds;
    }

    bool Time::operator != (const Time & time) const
    {
        return m_Microseconds != time.m_Microseconds;
    }

    bool Time::operator > (const Time & time) const
    {
        return m_Microseconds > time.m_Microseconds;
    }

    bool Time::operator < (const Time & time) const
    {
        return m_Microseconds < time.m_Microseconds;
    }

    bool Time::operator >= (const Time & time) const
    {
        return m_Microseconds >= time.m_Microseconds;
    }

    bool Time::operator <= (const Time & time) const
    {
        return m_Microseconds <= time.m_Microseconds;
    }

    Time Time::operator + (const Time & time) const
    {
        return Time(m_Microseconds + time.m_Microseconds);
    }

    Time & Time::operator += (const Time & time)
    {
        m_Microseconds += time.m_Microseconds;
        return *this;
    }

    Time Time::operator - (const Time & time) const
    {
        if (time.m_Microseconds > m_Microseconds)
        {
            return Time(0);
        }

        return Time(m_Microseconds - time.m_Microseconds);
    }

    Time & Time::operator -= (const Time & time)
    {
        if (time.m_Microseconds > m_Microseconds)
        {
            m_Microseconds = 0;
            return *this;
        }

        m_Microseconds -= time.m_Microseconds;
        return *this;
    }

    Time Time::operator * (const unsigned long long & value) const
    {
        return Time(m_Microseconds * value);
    }

    Time Time::operator / (const unsigned long long & value) const
    {
        return Time(m_Microseconds / value);
    }

    Time Time::operator % (const unsigned long long & value) const
    {
        return Time(m_Microseconds % value);
    }

    // Private functions
    Time::Time( const unsigned long long & microseconds ) :
        m_Microseconds( microseconds )
    {
    }

    // Functions for initializing time classes
    Time Seconds( const double & seconds )
    {
        return Time( static_cast<unsigned long long>( seconds * 1000000.0f ) );
    }

    Time Milliseconds( const unsigned long long & milliseconds )
    {
        return Time( milliseconds * 1000ULL );
    }

    Time Microseconds( const unsigned long long & microseconds )
    {
        return Time( microseconds );
    }

}
