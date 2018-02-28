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

        /**
        * @breif Base class of variable.
        *
        */
        class VariableBase
        {

        protected:

            /**
            * @breif Variable implementation class.
            *
            * @param data   Pointer to data to copy to variable.
            * @param size   Size of data.
            *
            */
            virtual bool CopyData(const void * data, const size_t size) = 0;

        };

        /**
        * @breif Variable implementation class.
        *
        */
        template <typename Type>
        class VariableImp : public VariableBase
        {

        protected:

            /**
            * @breif Constructor.
            *
            * @param value   Initialize variable by value.
            *
            */
            VariableImp();
            VariableImp(const Type & value);

            Type m_Value; ///< Value of variable.

        private:

            /**
            * @breif Variable implementation class.
            *
            * @param data   Pointer to data to copy to variable.
            * @param size   Size of data.
            *
            */
            virtual bool CopyData(const void * data, const size_t size);

        };

        #include <core/VariableImp.inl> ///< Include inline definitions.

    }

}


