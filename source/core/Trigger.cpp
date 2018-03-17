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

        Trigger::Trigger(const eType p_type) :
            type(p_type)
        {
        }

        Trigger::~Trigger()
        {
        }

        OnPeerPreConnectTrigger::OnPeerPreConnectTrigger(const bool p_NewPeer, Peer * p_Peer, const Time & p_receiveTime) :
            Trigger(OnPeerPreConnect),
            newPeer(p_NewPeer),
            peer(p_Peer),
            receiveTime(p_receiveTime)
        {
        }



        OnPeerConnectTrigger::OnPeerConnectTrigger(Peer * p_peer) :
            Trigger(OnPeerConnect),
            peer(p_peer)
        {
        }


        OnPeerDisconnectTrigger::OnPeerDisconnectTrigger(Peer * p_peer) :
            Trigger(OnPeerDisconnect),
            peer(p_peer)
        {
        }

    }

}
