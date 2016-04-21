/// \file
///
/// Deinterleaved location tests
#include <hm3/grid/hierarchical/tree/location/deinterleaved.hpp>
#include "test.hpp"

using namespace hm3;
using namespace tree;

template struct hm3::tree::location::deinterleaved<1, uint32_t>;
template struct hm3::tree::location::deinterleaved<2, uint32_t>;
template struct hm3::tree::location::deinterleaved<3, uint32_t>;
template struct hm3::tree::location::deinterleaved<1, uint64_t>;
template struct hm3::tree::location::deinterleaved<2, uint64_t>;
template struct hm3::tree::location::deinterleaved<3, uint64_t>;

int main() {
  {  // 1D (32 bit)
    test_location<1, 32>(location::deinterleaved<1, uint32_t>{});
  }

  {  // 2D (32 bit)
    test_location<2, 16>(location::deinterleaved<2, uint32_t>{});
  }

  {  // 3D (32_bit)
    test_location<3, 10>(location::deinterleaved<3, uint32_t>{});
  }
  {  // 1D (64 bit)
    test_location<1, 64>(location::deinterleaved<1, uint64_t>{});
  }

  {  // 2D (64 bit)
    test_location<2, 32>(location::deinterleaved<2, uint64_t>{});
  }

  {  // 3D (64_bit)
    test_location<3, 21>(location::deinterleaved<3, uint64_t>{});
  }

  test_location_2<location::deinterleaved>();
  return test::result();
}
