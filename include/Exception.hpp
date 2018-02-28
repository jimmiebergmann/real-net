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

#include <exception>
#include <string>

namespace Net
{

    /**
    * @breif Exception base class.
    *        Contains exception message.
    *
    */
    class Exception : public std::exception
    {

    public:

        /**
        * @breif Constructor.
        *        Initialize exception by message.
        *
        */
        Exception(const std::string & message);

        /**
        * @breif Get message of exception.
        *
        */
        const std::string & GetMessage() const;

        /**
        * @breif Same as Message, but returns a char pointer.
        *
        */
        virtual const char * what() const noexcept;

    private:

        std::string m_Message; ///< Message of exception.

    };


    /**
    * @breif System exception class.
    *        Contains exception message and error code.
    *
    */
    class SystemException : public Exception
    {

    public:

        /**
        * @breif Constructor.
        *        Initialize exception by message and error code.
        *
        */
        SystemException(const std::string & message, const size_t code);

        /**
        * @breif Get error code of exception.
        *
        */
        size_t GetCode() const;

    private:

        size_t m_Code; ///< Error code of exception.

    };


}
