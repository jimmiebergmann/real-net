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

            /**
            * @breif Connection packet types.
            *
            */
            static const unsigned char ConnectionTypeInit       = 0x00;
            static const unsigned char ConnectionTypeAccept     = 0x01;
            static const unsigned char ConnectionTypeReject     = 0x02;

            /**
            * @breif Synchronization packet types.
            *
            */
            static const unsigned char SynchronizationTypeInit  = 0x00;
            static const unsigned char SynchronizationTypeAck   = 0x01;


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
            * @breif Serialize sequence number from packet data
            *        and sets the internal sequence number.
            *
            */
            void SerializeSequenceNumber();

            unsigned char * data;           ///< Packet data.
            size_t          size;           ///< Size of packet. Not the number of allocated bytes.
            unsigned short  sequence;       ///< Sequence number of packet.
            Time            receiveTime;    ///< Time of when the packet were received.
            SocketAddress   address;        ///< Socket address of packet.

        private:

            /**
            * @breif Copy constructor. Not definied and not allowed.
            *
            */
            Packet(const Packet & packet);

        };

    }

}
