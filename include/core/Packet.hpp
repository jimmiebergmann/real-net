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
#include <Address.hpp>

namespace Net
{

    namespace Core
    {

        struct Packet
        {

            static const size_t MaxPacketSize                   = 2048; ///< Maximum size of each packet, in bytes.

            /**
            * @breif Packet types.
            *
            */
            static const unsigned char ConnectionType           = 0x00;
            static const unsigned char DisconnectionType        = 0x01;
            static const unsigned char SynchronizationType      = 0x02;
            static const unsigned char AcknowledgementType      = 0x03;
            static const unsigned char LinkingType              = 0x04;
            static const unsigned char ReplicationType          = 0x05;
            static const unsigned char MessageType              = 0x06;
            static const unsigned char TypeCount                = 0x07;  ///< Total number of packet types.

            /**
            * @breif Connection packet types.
            *
            */
            static const unsigned char ConnectionTypeInit       = 0x00;
            static const unsigned char ConnectionTypeAccept     = 0x01;
            static const unsigned char ConnectionTypeReject     = 0x02;
            static const unsigned char ConnectionTypeCount      = 0x03; ///< Total number of connection packet types.

            /**
            * @breif Synchronization packet types.
            *
            */
            static const unsigned char SynchronizationTypeInit  = 0x00;
            static const unsigned char SynchronizationTypeAck   = 0x01;
            static const unsigned char SynchronizationTypeCount = 0x02;  ///< Total number of connection packet types.


            // The follow declarations are non-static members of the packet structure.

            /**
            * @breif Constructor.
            *
            */
            Packet();

            /**
            * @breif Destructor.
            *
            */
            ~Packet();

            /**
            * @breif Serialize sequence number from packet data.
            *        The internal sequence number wont be set.
            *
            */
            unsigned short SerializeSequenceNumber() const;

            unsigned char * Data;           ///< Packet data.
            size_t          Size;           ///< Size of packet. Not the number of allocated bytes.
            unsigned short  Sequence;       ///< Sequence number of packet.
            Time            ReceiveTime;    ///< Time of when the packet were received.
            SocketAddress   Address;        ///< Socket address of packet.

        private:

            /**
            * @breif Copy constructor. Not definied and not allowed.
            *
            */
            Packet(const Packet & packet);

        };

    }

}
