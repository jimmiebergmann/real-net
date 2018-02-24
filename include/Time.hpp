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

#include <core/Build.hpp>

namespace Net
{

    /**
    * @breif Class representing time.
    *
    */
    class Time
    {

    public:

        /**
        * @breif Friend functions.
        *
        */
        friend Time Seconds(const double &);
        friend Time Milliseconds(const unsigned long long &);
        friend Time Microseconds(const unsigned long long &);

        /**
        * @breif Constructor.
        *
        */
        Time();

        /**
        * @breif Get the time in seconds.
        *
        * @return Time in seconds as a 64 bit floating point value.
        *
        */
        double AsSeconds() const;

        /**
        * @breif Get the time in milliseconds.
        *
        * @return Time in milliseconds as a 64 bit unsigned integer value.
        *
        */
        unsigned long long AsMilliseconds() const;

        /**
        * @breif Get the time in microseconds.
        *
        * @return Time in microseconds as a 64 bit unsigned integer value.
        *
        */
        unsigned long long AsMicroseconds() const;

        /**
        * @breif Value representing infinite time.
        *
        */
        static const Time Infinite;

        /**
        * @breif Value representing zero time.
        *
        */
        static const Time Zero;

        /**
        * @breif Equals to operator.
        *
        */
        bool operator == (const Time & time) const;

        /**
        * @breif Not equals to operator.
        *
        */
        bool operator != (const Time & time) const;

        /**
        * @breif Greater than operator.
        *
        */
        bool operator > (const Time & time) const;

        /**
        * @breif Less than operator.
        *
        */
        bool operator < (const Time & time) const;

        ////////////////////////////////////////////////////////////////
        /// \brief Greater than or equal to operator
        ///
        ////////////////////////////////////////////////////////////////
        /**
        * @breif Equals to operator
        *
        */
        bool operator >= (const Time & time) const;

        /**
        * @breif Less or equals to operator.
        *
        */
        bool operator <= (const Time & time) const;

        /**
        * @breif Addition operator.
        *
        */
        Time operator + (const Time & time) const;

        /**
        * @breif Addition operator of time.
        *
        */
        Time & operator += (const Time & time);

        /**
        * @breif Subtraction operator.
        *
        */
        Time operator - (const Time & time) const;

         /**
        * @breif Subtraction operator of time.
        *
        */
        Time & operator -= (const Time & time);

        /**
        * @breif Multiply operator.
        *
        */
        Time operator * (const unsigned long long & value) const;

        /**
        * @breif Division operator.
        *
        */
        Time operator / (const unsigned long long & value) const;

        /**
        * @breif Modulus operator.
        *
        */
        Time operator % (const unsigned long long & value) const;

    private:


        /**
        * @breif Constructor.
        *        Initialize time as microseconds.
        *
        */
        Time(const unsigned long long & microseconds);

        unsigned long long m_Microseconds;	///< Time in microseconds.

    };

    /**
    * @breif Functions for initializing a time class
    *
    * @param seconds Time in seconds.
    * @param milliseconds Time in milliseconds.
    * @param microseconds Time in microseconds.
    *
    */
    Time Seconds(const double & seconds);
    Time Milliseconds( const unsigned long long & milliseconds);
    Time Microseconds( const unsigned long long & microseconds);

}
