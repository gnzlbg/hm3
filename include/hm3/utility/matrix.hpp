#pragma once
/// \file
///
/// Matrix types
#include <hm3/utility/matrix/matrix.hpp>

namespace hm3 {

template <uint_t N>
using num_a = Eigen::Matrix<num_t, 1, N>;

}  // namespace hm3
