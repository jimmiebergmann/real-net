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
#include <functional>


namespace Net
{

    class Server : public EntityManager, public Core::ServerImp
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
        *        Host(1) is the simply way of hosting the server.
        *        Host(2) allows additional settings. port and maxConnections param will overload settings values.
        *        Host(3) start server via settings.
        *
        * @param port               Hosting port.
        * @param maxConnections     Maximum number of concurrent connections.
        * @param settings           Passed server settings.
        *
        * @throw SystemException    If failing to create server port.
        * @throw Exception          If calling Host/Stop in wrong order or
        *                           starting server while it's stopping.
        *
        */
        void Host(const unsigned short port, const size_t maxConnections);
        void Host(const unsigned short port, const size_t maxConnections, const Settings & settings);
        void Host(const Settings & settings);

        /**
        * @breif Stop hosted server. No action if not hosted.
        *
        */
        void Stop();

        /**
        * @breif Disconnect peer from server.
        *
        */
        virtual void DisconnectPeer(std::shared_ptr<Peer> peer);


        void SetPeerTimeout(std::shared_ptr<Peer> peer, const Time & timeout);

        /**
        * @breif Set the "On peer pre connect" trigger. Overloads the virtual function.
        *
        * @throw Exception If function already is set, or if the server is currently hosted.
        *
        */
        void SetOnPeerPreConnect(const std::function<bool(std::shared_ptr<Net::Peer> peer)> & function);

        /**
        * @breif Set the "On peer connect" trigger. Overloads the virtual function.
        *
        * @throw Exception If function already is set, or if the server is currently hosted.
        *
        */
        void SetOnPeerConnect(const std::function<void(std::shared_ptr<Net::Peer> peer)> & function);

        /**
        * @breif Set the "On peer disconnect" trigger. Overloads the virtual function.
        *
        * @throw Exception If function already is set, or if the server is currently hosted.
        *
        */
        void SetOnPeerDisconnect(const std::function<void(std::shared_ptr<Net::Peer> peer, const Peer::eReason reason)> & function);

        /**
        * @breif Get current server settings.
        *
        */
        const Settings & GetSettings() const;

    private:

        /**
        * @breif Trigger function called before connection with peer is established.
        *        Overload and return true to accept peer. Return false to reject.
        *
        */
        virtual bool OnPeerPreConnect(std::shared_ptr<Net::Peer> peer);

        /**
        * @breif Trigger function called when connection with new peer has been established.
        *
        */
        virtual void OnPeerConnect(std::shared_ptr<Net::Peer> peer);

        /**
        * @breif Trigger function called if peer disconnects.
        */
        virtual void OnPeerDisconnect(std::shared_ptr<Net::Peer> peer, const Peer::eReason reason);

    };

}
