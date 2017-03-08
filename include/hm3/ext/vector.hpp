#pragma once
/// \file
///
/// HM3's vector types.
#include <boost/container/vector.hpp>
#include <vector>

namespace hm3 {

template <typename T, typename A>
using std_vector = std::vector<T, A>;

}  // namespace hm3
