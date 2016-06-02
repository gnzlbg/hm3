#pragma once
/// \file
///
/// Line segment centroid
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Line centroid
template <dim_t Nd> constexpr point<Nd> centroid(segment<Nd> const& l) {
  point<Nd> v;
  v() = l.x_0() + 0.5 * (l.x_1() - l.x_0());
  return v;
}

}  // namespace geometry
}  // namespace hm3
