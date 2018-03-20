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

#include <Address.hpp>
#include <Time.hpp>

namespace Net
{


    /**
    * @breif Server/client settings.
    *
    *        Passed to Host and Connect function.
    *        It's not possible to change the settings when the server is hosted or connected to server.
    *
    */
    class Settings
    {

    public:

        /**
        * @breif Constructor.
        *        Multiple constructors, relevant for Server/Client.
        *
        * @param port           Hosting port.
        * @param maxConnections Maximum number of concurrent connections.
        * @param host           Host address.
        *                       Also specifies ip family. Ipv4/Ipv6/Any.
        *                       "Any" will allow both Ipv4 and Ipv6 connections.
        * @param timeout        Peer/server timeout, nessecary packets are not received in time if reached.
        *
        */
        Settings();

        Settings(const unsigned short   port,
                 const size_t           maxConnections = 0,
                 const Address &        host = Address(Address::Any),
                 const Time &           timeout = Seconds(30),
                 const Time &           handshakeTimeout = Seconds(10),
                 const size_t           latencySamples = 7);

        Settings(const Address &        host,
                 const unsigned short   port,
                 const Time &           timeout = Seconds(30),
                 const Time &           handshakeTimeout = Seconds(10),
                 const size_t           latencySamples = 7);



        unsigned short  port;               ///< Hosting port.
        size_t          maxConnections;     ///< Maximum number of concurrent connections.
        Address         host;               ///< Host address and ip family of server. "Any" will allow both Ipv4 and Ipv6 connections.
        Time            timeout;            ///< Peer/server timeout, nessecary packets are not received in time if reached.
        Time            handshakeTimeout;   ///< Number of time until handshake timeout.
        size_t          latencySamples;     ///< Number of samples used for latency calculation.

    };

}
