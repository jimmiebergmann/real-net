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

#include <core/Trigger.hpp>

namespace Net
{

    namespace Core
    {

        Trigger::Trigger(const eType p_Type) :
            type(p_Type)
        {
        }

        Trigger::~Trigger()
        {
        }

        PeerTrigger::PeerTrigger(std::shared_ptr<Peer> p_Peer, const eType type) :
            Trigger(type),
            peer(p_Peer)
        {
        }

        PeerTrigger::~PeerTrigger()
        {
        }

        OnPeerPreConnectTrigger::OnPeerPreConnectTrigger(std::shared_ptr<Peer> p_Peer, const Time & p_ReceiveTime) :
            PeerTrigger(p_Peer, OnPeerPreConnect),
            receiveTime(p_ReceiveTime)
        {

        }

        OnPeerConnectTrigger::OnPeerConnectTrigger(std::shared_ptr<Peer> p_Peer) :
            PeerTrigger(p_Peer, OnPeerConnect)
        {
        }


        OnPeerDisconnectTrigger::OnPeerDisconnectTrigger(std::shared_ptr<Peer> p_Peer,
                                                         const Peer::Disconnect::eReason p_Reason,
                                                         const bool p_TriggerFunction, const bool p_SendResponse) :
            PeerTrigger(p_Peer, OnPeerDisconnect),
            reason(p_Reason),
            triggerFunction(p_TriggerFunction),
            sendResponse(p_SendResponse)
        {
        }

    }

}
