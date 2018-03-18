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

#include <core/Latency.hpp>
#include <algorithm>

namespace Net
{

    namespace Core
    {

        Latency::Latency(const size_t maxSize) :
            m_MaxSize(std::max<size_t>(1, maxSize)),
            m_Median(0),
            m_ReplacePos(0)
        {
        }

        void Latency::Add(const Time & time)
        {
            if(m_Latencies.size() < m_MaxSize)
            {
                m_Latencies.push_back(time);
            }
            else
            {
                m_Latencies[m_ReplacePos] = time;

                m_ReplacePos++;
                if(m_ReplacePos == m_MaxSize)
                {
                    m_ReplacePos = 0;
                }
            }

            // Calculate median.
            std::vector<Time> sorted = m_Latencies;

            const bool oddCount = static_cast<bool>(m_Latencies.size() % 2);
            const size_t middle = m_Latencies.size() / 2;
            std::partial_sort(sorted.begin(), sorted.begin() + (middle + 1), sorted.end());

            if(!oddCount)
            {
                m_Median = (sorted[middle-1] + sorted[middle]).AsMicroseconds() / 2ULL;
                return;
            }

            m_Median = sorted[middle].AsMicroseconds();
        }

        void Latency::Get(Time & time) const
        {
            time = Microseconds(m_Median);
        }

        size_t Latency::GetCount() const
        {
            return m_Latencies.size();
        }

        size_t Latency::GetSampleCount() const
        {
            return m_MaxSize;
        }

    }

}
