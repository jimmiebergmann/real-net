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

template <typename Item>
DataQueue<Item>::DataQueue(Item defaultItem, const bool notify) :
    m_DefaultItem(defaultItem),
    m_Notify(notify)
{
}

template <typename Item>
DataQueue<Item>::~DataQueue()
{
    while(m_Queue.size())
    {
        m_Queue.pop();
    }
}

template <typename Item>
void DataQueue<Item>::Push(Item item)
{
    Core::SafeGuard sf_mutex(m_Mutex);

    m_Queue.push(item);

    if(m_Notify)
    {
        semaphore.NotifyOne();
    }
}

template <typename Item>
Item DataQueue<Item>::Fetch()
{
    Core::SafeGuard sf_mutex(m_Mutex);

    if(m_Queue.size() == 0)
    {
        return m_DefaultItem;
    }

    Item item = m_Queue.front();
    m_Queue.pop();

    return item;
}

template <typename Item>
void DataQueue<Item>::Clear()
{
    Core::SafeGuard sf_mutex(m_Mutex);

    while(m_Queue.size())
    {
        m_Queue.pop();
    }
}

template <typename Item>
void DataQueue<Item>::Clear(const std::function<void(Item item)> clearFunction)
{
    Core::SafeGuard sf_mutex(m_Mutex);

    while(m_Queue.size())
    {
        Item item = m_Queue.front();
        clearFunction(item);
        m_Queue.pop();
    }
}
