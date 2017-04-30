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

#pragma once

#include <atomic>   // std::atomic embedded.
#include <cstddef>  // For size_t
#include <ostream>  // std::ostream inlined.
#include <memory>   // std::unique_ptr embedded.
#include <thread>   // std::thread embedded.
#include <vector>   // std::vector embedded.

#include "Stopwatch.h"  // Stopwatch embedded.
#include "ThreadSynchronizer.h" // Template parameter.


namespace BitFunnel {
    class QueryLogRunnerBase {
    public:
        class QueryProcessorBase;

        QueryLogRunnerBase(size_t queryCount);

        // virtual destructor for subclasses.
        virtual ~QueryLogRunnerBase() {};

        void Go(size_t threadCount);

        void Write(std::ostream& out) const;

        virtual std::unique_ptr<QueryProcessorBase> CreateProcessor() = 0;

        class QueryProcessorBase {
        public:
            QueryProcessorBase(QueryLogRunnerBase& runner);

            virtual ~QueryProcessorBase() {};

            void Entry();

            static void ThreadEntryPoint(void *data);

            virtual void ProcessLog(std::atomic<long long>& queriesRemaining) = 0;

        protected:
            Stopwatch m_stopwatch;
            QueryLogRunnerBase& m_runnerBase;
        };

        class Result
        {
        public:
            Result()
                : m_succeeded(false),
                  m_matchCount(0),
                  m_time(0.0)
            {
            }

            Result(size_t matchCount, double time)
                : m_succeeded(true),
                  m_matchCount(matchCount),
                  m_time(time)
            {
            }

            bool Succeeded() const
            {
                return m_succeeded;
            }

            size_t MatchCount() const
            {
                return m_matchCount;
            }

            size_t Timet() const
            {
                return m_time;
            }

            void Write(std::ostream& out) const
            {
                out << (m_succeeded?"true":"false");
                out << "," << m_matchCount;
                out << "," << m_time << std::endl;
            }

        private:
            bool m_succeeded;
            size_t m_matchCount;
            double m_time;
        };

        std::vector<std::unique_ptr<QueryProcessorBase>> m_processors;
        std::vector<std::thread> m_threads;

        std::unique_ptr<ThreadSynchronizer> m_warmupSynchronizer;
        std::unique_ptr<ThreadSynchronizer> m_measureSynchronizer;
        std::unique_ptr<ThreadSynchronizer> m_finishedSynchronizer;

        // Counters must be signed in order to check whether decrement
        // resulted in a negative number.
        std::atomic<long long> m_warmupQueryCount;
        std::atomic<long long> m_measureQueryCount;

        std::vector<Result> m_results;
    };
}

//#endif //INDEX_PARTITIONING_QUERYLOGRUNNER_H
