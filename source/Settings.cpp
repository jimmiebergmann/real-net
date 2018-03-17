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

#include <Settings.hpp>

namespace Net
{

    Settings::Settings() :
        port(0),
        maxConnections(0),
        host(Address::Zero),
        timeout(Seconds(30))
    {
    }

    Settings::Settings(const unsigned short   p_Port,
                       const size_t           p_MaxConnections,
                       const Address &        p_Host,
                       const Time &           p_Timeout) :
        port(p_Port),
        maxConnections(p_MaxConnections),
        host(p_Host),
        timeout(p_Timeout)
    {
    }

    Settings::Settings(const Address &        p_Host,
                       const unsigned short   p_Port,
                       const Time &           p_Timeout) :
        port(p_Port),
        maxConnections(0),
        host(p_Host),
        timeout(p_Timeout)
    {
    }

}
