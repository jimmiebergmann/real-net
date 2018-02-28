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

#include <core/ServerImp.hpp>
#include <EntityManager.hpp>
#include <Address.hpp>


namespace Net
{

    class Server : public EntityManager, private Core::ServerImp
    {

    public:


        /**
        * @breif Constructor.
        *
        */
        Server();

        /**
        * @breif Destructor.
        *
        */
        virtual ~Server();

        /**
        * @breif Host server. No action if server already is hosted.
        *
        * @param port Hosting port.
        * @param family Specify connection family. Ipv4/Ipv6/Any.
        *               "Any" will allow both Ipv4 and Ipv6 connections.
        *
        * @throw SystemException    If failing to create server port.
        * @throw Exception          If calling Stop/Start in wrong order or
        *                           if starting server while it's stopping.
        *
        */
        void Host(const unsigned short port, Address::eType family = Address::Any);

        /**
        * @breif Stop hosted server. No action if not hosted.
        *
        */
        void Stop();

        // Virtual functions
        virtual bool OnPeerPreConnect(Peer & peer);
        virtual void OnPeerConnect(Peer & peer);
        virtual void OnPeerDisconnect(Peer & peer);

        REALNET_TEST_FRIEND ///< Allow private tests.

    };

}
