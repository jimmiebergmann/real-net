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
#include <Address.hpp>

namespace Net
{

    /**
    * @breif Peer class describing connecting peer to server.
    *
    */
    class Peer
    {

    public:

        friend class Server;

        /**
        * @breif Get id of peer.
        *
        */
        unsigned short GetId() const;

        /**
        * @breif Get socket address of peer.
        *
        */
        const SocketAddress & GetSocketAddress() const;

    protected:

        /**
        * @breif Constructor.
        *
        */
        Peer(const unsigned short id, const SocketAddress & socketAddress);

    private:

        unsigned short  m_Id;               ///< Id of peer.
        SocketAddress   m_SocketAddress;    ///< Ip and port of peer.

    };

}
