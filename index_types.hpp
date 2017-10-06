#pragma once

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "binary_freq_collection.hpp"
#include "block_codecs.hpp"
#include "block_freq_index.hpp"
#include "freq_index.hpp"
#include "partitioned_sequence.hpp"
#include "positive_sequence.hpp"
#include "uniform_partitioned_sequence.hpp"

namespace quasi_succinct {

typedef freq_index<compact_elias_fano, positive_sequence<strict_elias_fano>>
    ef_index;

typedef freq_index<indexed_sequence, positive_sequence<>> single_index;

typedef freq_index<
    uniform_partitioned_sequence<>,
    positive_sequence<uniform_partitioned_sequence<strict_sequence>>>
    uniform_index;

typedef freq_index<partitioned_sequence<>,
                   positive_sequence<partitioned_sequence<strict_sequence>>>
    opt_index;

typedef block_freq_index<quasi_succinct::optpfor_block> block_optpfor_index;

typedef block_freq_index<quasi_succinct::varint_G8IU_block> block_varint_index;

typedef block_freq_index<quasi_succinct::interpolative_block>
    block_interpolative_index;

typedef block_freq_index<quasi_succinct::qmx_block> block_qmx_index;
}

#define QS_INDEX_TYPES                                                         \
  (ef)(single)(uniform)(opt)(block_optpfor)(block_varint)(                     \
      block_interpolative)(block_qmx)
