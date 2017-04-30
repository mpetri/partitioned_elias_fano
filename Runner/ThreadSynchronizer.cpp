// The MIT License (MIT)

// Copyright (c) 2016, Microsoft

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "ThreadSynchronizer.h"

namespace BitFunnel
{
    ThreadSynchronizer::ThreadSynchronizer(size_t threadCount)
        : m_threadCount(threadCount)
    {
    }


    void ThreadSynchronizer::Wait()
    {
        std::unique_lock<std::mutex> lock(m_lock);

        --m_threadCount;
        if (m_threadCount == 0)
        {
            m_stopwatch.Reset();
            m_wakeCondition.notify_all();
        }
        else
        {
            while (m_threadCount > 0)
            {
                m_wakeCondition.wait(lock);
            }
        }
    }


    double ThreadSynchronizer::GetElapsedTime() const
    {
        return m_stopwatch.ElapsedTime();
    }
}