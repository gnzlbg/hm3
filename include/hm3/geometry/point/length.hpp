#pragma once
/// \file
///
/// Vector length
#include <hm3/geometry/point/vector.hpp>

namespace hm3 {
namespace geometry {

/// Vector length
template <dim_t Nd> constexpr num_t length(vector<Nd> const& p) {
  return p().norm();
}

}  // namespace geometry
}  // namespace hm3
