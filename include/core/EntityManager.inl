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

template <typename EntityClass>
EntityLinkage<EntityClass>::~EntityLinkage()
{

}

template <typename EntityClass>
EntityLinkage<EntityClass>::EntityLinkage()
{
}

template <typename EntityClass>
template <typename Type>
EntityLinkage<EntityClass> & EntityLinkage<EntityClass>::LinkVariable(const std::string & name, Variable<Type> EntityClass::* variable)
{
    Core::VariableBase Entity::* pVariable = reinterpret_cast<Core::VariableBase Entity::*>(variable);

    auto it = m_VariableLinks.find(name);
    if(it != m_VariableLinks.end() && it->second != pVariable)
    {
        it->second = pVariable;
        return *this;
    }

    m_VariableLinks.insert({name, pVariable});
    return *this;
}

template <typename EntityClass>
bool EntityLinkage<EntityClass>::IsLinked(const std::string & name) const
{
    return m_VariableLinks.find(name) != m_VariableLinks.end();
}


template <typename EntityClass>
EntityLinkage<EntityClass> & EntityManager::LinkEntity(const std::string & name)
{
    std::string className = typeid(EntityClass).name();
    auto it = m_EntityLinks.find(name);
    if(it != m_EntityLinks.end())
    {
        if(typeid(EntityClass).hash_code() != it->second->m_TypeHash || it->second->m_ClassName != className)
        {
            throw Exception("Entity is already linked, but not of same type.");
        }
        return *static_cast<EntityLinkage<EntityClass>*>(it->second);
    }

    EntityLinkage<EntityClass> * pLinkage = new EntityLinkage<EntityClass>;
    pLinkage->m_TypeHash = typeid(EntityClass).hash_code();
    pLinkage->m_ClassName = className;
    pLinkage->m_AllocationFunction = &AllocateEntity<EntityClass>;
    m_EntityLinks.insert({name, pLinkage});

    return *pLinkage;
}

template <typename EntityClass>
EntityClass * EntityManager::CreateEntity(const std::string & name)
{
    Entity * pEntity = nullptr;

    return static_cast<EntityClass*>(pEntity);
}
