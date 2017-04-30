/// The MIT License (MIT)

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

#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>

#include "Stopwatch.h"


namespace BitFunnel
{
    //*************************************************************************
    //
    // ThreadSynchronizer
    //
    // ThreadSynchronizer reduces the lag between the times various threads
    // start to do work. Intended for aiding in performance measurement where
    // we don't want to capture the OS thread startup time.
    //
    // Suspends the first n - 1 threads to call Wait().
    // Wakes all threads on the nth call to Wait().
    //
    //*************************************************************************
    class ThreadSynchronizer
    {
    public:
        ThreadSynchronizer(size_t threadCount);

        void Wait();

        double GetElapsedTime() const;

    private:
        size_t m_threadCount;
        std::mutex m_lock;
        std::condition_variable m_wakeCondition;

        Stopwatch m_stopwatch;
    };
}

//#endif //INDEX_PARTITIONING_THREADSYNCHRONIZER_H
