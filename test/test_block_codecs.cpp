#define BOOST_TEST_MODULE block_codecs

#include "block_codecs.hpp"
#include "succinct/test_common.hpp"
#include <cstdlib>
#include <vector>

template <typename BlockCodec> void test_block_codec() {
  std::vector<size_t> sizes = {1, 16, BlockCodec::block_size - 1,
                               BlockCodec::block_size};
  for (auto size : sizes) {
    std::vector<uint32_t> values(size);
    std::generate(values.begin(), values.end(),
                  []() { return (uint32_t)rand() % (1 << 24); });

    for (size_t tcase = 0; tcase < 2; ++tcase) {
      // test both undefined and given sum_of_values
      uint32_t sum_of_values(-1);
      if (tcase == 1) {
        sum_of_values = std::accumulate(values.begin(), values.end(), 0);
      }
      std::vector<uint8_t> encoded;
      BlockCodec::encode(values.data(), sum_of_values, values.size(), encoded);

      std::vector<uint32_t> decoded(values.size());
      uint8_t const *out = BlockCodec::decode(encoded.data(), decoded.data(),
                                              sum_of_values, values.size());

      BOOST_REQUIRE_EQUAL(encoded.size(), out - encoded.data());
      BOOST_REQUIRE_EQUAL_COLLECTIONS(values.begin(), values.end(),
                                      decoded.begin(), decoded.end());
    }
  }
}

BOOST_AUTO_TEST_CASE(block_codecs) {
  test_block_codec<quasi_succinct::qmx_block>();
  test_block_codec<quasi_succinct::optpfor_block>();
  test_block_codec<quasi_succinct::varint_G8IU_block>();
  test_block_codec<quasi_succinct::interpolative_block>();
}
