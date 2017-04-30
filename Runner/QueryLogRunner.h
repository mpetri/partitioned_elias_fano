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

#include <cstddef>                  // size_t parameter.
#include <iostream>
#include <string>                   // std::string in typedef.
#include <vector>                   // std::vector parameter.

#include "queries.hpp"              // inline code uses quasi_succinct::and_query.

#include "QueryLogRunnerBase.h"     // Base class.


namespace quasi_succinct {
    typedef uint32_t term_id_type;
    typedef std::vector<term_id_type> term_id_vec;
}


namespace BitFunnel
{
    template <typename INDEX>
    class QueryLogRunner : public QueryLogRunnerBase {
    public:
        QueryLogRunner(std::vector<quasi_succinct::term_id_vec> const &queries,
                       INDEX const & index)
            : QueryLogRunnerBase(queries.size()),
              m_queries(queries),
              m_index(index)
        {
        }


        virtual std::unique_ptr<QueryProcessorBase> CreateProcessor() override
        {
            return std::unique_ptr<QueryProcessorBase>(new QueryProcessor(*this));
        }


        class QueryProcessor : public QueryLogRunnerBase::QueryProcessorBase
        {
        public:
            QueryProcessor(QueryLogRunner& runner)
                : QueryProcessorBase(runner),
                  m_runner(runner)
            {
            }

            virtual void ProcessLog(std::atomic<long long>& queriesRemaining) override
            {
                quasi_succinct::and_query<false> queryOperator;

                long long queryCount = m_runner.m_queries.size();

                while (true)
                {
                    long long query = --queriesRemaining;
                    if (query < 0)
                    {
                        break;
                    }

                    size_t queryIndex = queryCount - (query % queryCount) - 1;

                    m_stopwatch.Reset();

                    // Process query.
                    size_t matchCount = queryOperator(m_runner.m_index, m_runner.m_queries[queryIndex]);
                    m_runnerBase.m_results[queryIndex] = Result(matchCount, m_stopwatch.ElapsedTime());
                }
            }

        private:
            QueryLogRunner& m_runner;
        };

    private:
        std::vector<quasi_succinct::term_id_vec> const & m_queries;
        INDEX const & m_index;
    };
}