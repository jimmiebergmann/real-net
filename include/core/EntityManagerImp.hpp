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
#include <Entity.hpp>
#include <typeinfo>
#include <string>
#include <map>

namespace Net
{

    class EntityManager;

    namespace Core
    {

        class EntityLinkageImp
        {

        public:

            friend class Net::EntityManager;

        protected:

            Entity*(*AllocationFunction)();    ///< Pointer to entity allocation function.
            //std::string ClassName;           /// Raw name of entity class.

        };

        class EntityManagerImp
        {

        protected:

            /**
            * @breif Destructor.
            *
            */
            ~EntityManagerImp();

            /**
            * @breif Function for allocating a new entity.
            *
            */
            template <typename T>
            static Entity * AllocateEntity();

            std::map<std::string, EntityLinkageImp *>   m_EntityLinkages;   ///< Map of all entity links.
            std::map<EntityIdType, Entity *>            m_Entities;         ///< Map of all entities.

        };

         #include <core/EntityManagerImp.inl> ///< Include inline definitions.

    }

}


