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

    namespace Core
    {

        namespace Packet
        {

            /**
            * @breif Enumerator of all packet types.
            *
            */
            enum eType
            {
                ConnectionType          = 0,
                DisconnectionType       = 1,
                SynchronizationType     = 2,
                AcknowledgementType     = 3,
                LinkingType             = 4,
                ReplicationType         = 5,
                MessageType             = 6
            };
            const size_t TypeCount      = 7;  ///< Total number of packet types.

            /**
            * @breif Enumerator of different connection packet types.
            *
            */
            enum eConnectionType
            {
                ConnectionTypeInit              = 0,
                ConnectionTypeAccept            = 1,
                ConnectionTypeReject            = 2
            };
            const size_t ConnectionTypeCount    = 3;        ///< Total number of connection packet types.

            const size_t ConnectionInitPacketSize   = 4;    ///< Total number of bytes in init packet.
            const size_t ConnectionAcceptPacketSize = 24;   ///< Total number of bytes in accept packet.
            const size_t ConnectionRejectPacketSize = 5;    ///< Total number of bytes in reject packet. Excluding reason size.

            /**
            * @breif Enumerator of different synchronization packet types.
            *
            */
            enum eSynchronizationType
            {
                SynchronizationTypeInit             = 0,
                SynchronizationTypeAck              = 1,
            };
            const size_t SynchronizationTypeCount   = 2;  ///< Total number of connection packet types.


        }

    }

}
