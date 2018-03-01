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

#include <core/EntityManagerImp.hpp>

namespace Net
{

    /**
    * @breif Entity linkage class.
    *        Linking entity classes with entity manager.
    *
    */
    template <typename EntityClass>
    class EntityLinkage final : public Core::EntityLinkageImp
    {

    public:

        friend class EntityManager;

        /**
        * @breif Link variable to entity linkage.
        *        If name already is linked, the new variable linkage will replace the old.
        *
        * @param name       Name of variable.
        * @param variable   Pointer to entity variable.
        *
        */
        template <typename Type>
        EntityLinkage<EntityClass> & LinkVariable(const std::string & name, Variable<Type> EntityClass::* variable);

        /**
        * @breif Check if given name of variable is linked.
        *
        */
        bool IsLinked(const std::string & name) const;

    protected:

        /**
        * @breif Destructor.
        *
        */
        ~EntityLinkage();

    private:

        /**
        * @breif Constructor.
        *
        */
        EntityLinkage();

        /**
        * @breif Copy constructor.
        *
        */
        EntityLinkage(const EntityLinkage & entityLinkage);
    };


    /**
    * @breif Entity manager class.
    *
    */
    class EntityManager : public Core::EntityManagerImp
    {

    public:

        /**
        * @breif Destructor.
        *
        */
        ~EntityManager();

        /**
        * @breif Link entity to a certain name.
        *
        * @throw Exception  If name already is linked, but not of same EntityClass type.
        *
        */
        template <typename EntityClass>
        EntityLinkage<EntityClass> & LinkEntity(const std::string & name);

        /**
        * @breif Unlink entity by given name.
        *
        */
        void UnlinkEntity(const std::string & name);

        /**
        * @breif Check if given name of entity is linked.
        *
        */
        bool IsLinked(const std::string & name) const;

        /**
        * @breif Create new entity.
        *
        */
        template <typename EntityClass>
        EntityClass * CreateEntity(const std::string & name);

    protected:

        /**
        * @breif Constructor.
        *
        */
        EntityManager();

        /**
        * @breif Read incoming entity message.
        *
        * @param message    Pointer to message data.
        * @param size       Data size of message.
        *
        */
        void ReadMessage(const unsigned char * message, const size_t size);

        /**
        * @breif Write outgoing entity message of changes.
        *
        * @param size       Size of created message.
        * @param entities   Number of entities in message.
        *
        * @return Pointer to created message. nullptr on internal error.
        *
        */
        unsigned char * WriteMessage(const size_t & size, const size_t & entities);

    private:

        /**
        * @breif Copy constructor.
        *
        */
        EntityManager(const EntityManager & entityManager);

        REALNET_TEST_FRIEND ///< Allow private tests.

    };

    #include <inline/EntityManager.inl> ///< Include inline definitions.

}
