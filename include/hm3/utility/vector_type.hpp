#pragma once
/// \file
///
/// HM3's vector type
#include <vector>
// #include <boost/container/vector.hpp>

namespace hm3 {

template <typename T, typename A>
using std_vector = std::vector<T, A>;

// template <typename T, typename A>
// using std_vector = boost::container::vector<T, A>;

}  // namespace hm3
