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

#include <boost/preprocessor/seq/for_each.hpp>
#include <succinct/mapper.hpp>

#include "index_types.hpp"

#include "QueryLogRunner.h"


using namespace quasi_succinct;

std::vector<term_id_vec> ReadQueries(char const * queryFileName)
{
    std::vector<term_id_vec> queries;
    std::ifstream input(queryFileName);
    std::string line;
    term_id_vec queryVector;
    while (std::getline(input, line))
    {
        queryVector.clear();
        std::istringstream lineStream(line);
        term_id_type term;
        while (lineStream >> term)
        {
            queryVector.push_back(term);
        }
        queries.push_back(queryVector);
    }

    return queries;
}


template <typename INDEX>
void PerfTest(const char* indexFile,
              std::vector<quasi_succinct::term_id_vec> const& queries,
              std::string const& indexType,
              size_t maxThreads,
              char const * outFile)
{
    INDEX index;
    logger() << "Loading index from " << indexFile << std::endl;
    boost::iostreams::mapped_file_source m(indexFile);
    succinct::mapper::map(index, m, succinct::mapper::map_flags::warmup);

    BitFunnel::QueryLogRunner<INDEX> runner(queries, index);

    logger() << "Performing " << indexType << " queries" << std::endl;

    for (size_t threadCount = 1; threadCount <= maxThreads; ++threadCount)
    {
        runner.Go(threadCount);

        if (outFile == nullptr)
        {
            runner.WriteSummary(std::cout);
        }
        else
        {
            std::stringstream statistics;
            statistics << outFile << "-statistics-" << threadCount << ".csv";
            logger() << "  " << statistics.str() << std::endl;
            std::ofstream outStatistics(statistics.str());
            runner.WriteQueryStatistics(outStatistics);
            outStatistics.close();

            std::stringstream summary;
            summary << outFile << "-summary-" << threadCount << ".txt";
            logger() << "  " << summary.str() << std::endl;
            std::ofstream outSummary(summary.str());
            runner.WriteSummary(outSummary);
            outSummary.close();

            runner.WriteSummary(std::cout);
        }
    }
}


int main(int argc, const char** argv)
{
    if (argc < 5 || argc > 6)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " type index queries maxthreads [output]" << std::endl;
        std::cout << std::endl;
        std::cout << "  type     Index type. Choices are ef, single, uniform, opt" << std::endl;
        std::cout << "           block_optpfor, block_varint, and block_interpolative." << std::endl;
        std::cout << "  index    Index file name." << std::endl;
        std::cout << "  queries  Query log file name. One query per line." << std::endl;
        std::cout << "           Terms expressed as non-negative integers." << std::endl;
        std::cout << "  threads  Run performance trials with thread counts in [1..threads]." << std::endl;
        std::cout << "  output   Base name for output files containing performance" << std::endl;
        std::cout << "           measurements for various thread counts. If output is" << std::endl;
        std::cout << "           omitted, summary data will be printed to stdout." << std::endl;
        std::cout << "  " << std::endl;
    }
    else
    {
        std::string indexType = argv[1];
        char const * indexFile = argv[2];
        char const * queryFile = argv[3];
        size_t maxThreads = std::stol(argv[4]);
        char const * outFile = nullptr;
        if (argc == 6)
        {
            outFile = argv[5];
        }

        std::vector<term_id_vec> queries(ReadQueries(queryFile));
        logger() << "Loaded " << queries.size() << " queries from " << queryFile << std::endl;

        if (false) {
#define LOOP_BODY(R, DATA, T)                                   \
        } else if (indexType == BOOST_PP_STRINGIZE(T)) {             \
            PerfTest<BOOST_PP_CAT(T, _index)>                   \
                (indexFile, queries, indexType, maxThreads, outFile); \
            /**/

            BOOST_PP_SEQ_FOR_EACH(LOOP_BODY, _, QS_INDEX_TYPES);
#undef LOOP_BODY
        } else {
            logger() << "ERROR: Unknown type " << indexType << std::endl;
        }
    }
}

