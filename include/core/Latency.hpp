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

#include <Time.hpp>
#include <vector>
#include <atomic>

namespace Net
{

    namespace Core
    {

        /**
        * @breif Helper class for getting latency median.
        *        The oldes latency is being replaced by new one if maxSize is reached.
        *
        */
        class Latency
        {

        public:

            /**
            * @breif Constructor.
            *
            */
            Latency(const size_t maxSize);

            /**
            * @breif Add latency time.
            *
            */
            void Add(const Time & time);

            /**
            * @breif Get current latency median.
            *
            */
            void Get(Time & time) const;

            /**
            * @breif Get current latency sample.
            *
            */
            size_t GetCount() const;

            /**
            * @breif Get maximum number of samples.
            *
            */
            size_t GetSampleCount() const;

        private:

            std::vector<Time>               m_Latencies;    ///< Vector of all latencies, used for calculating median.
            const size_t                    m_MaxSize;      ///< Maximum number of latencies for calulating.
            std::atomic<unsigned long long> m_Median;       ///< Current median.
            size_t                          m_ReplacePos;   ///< Position of current latency to replace.

        };

    }
}
