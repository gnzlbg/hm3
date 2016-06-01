#pragma once
/// \file
///
/// Line centroid
#include <hm3/geometry/line/line.hpp>

namespace hm3 {
namespace geometry {

/// Line centroid
template <dim_t Nd> constexpr point<Nd> centroid(line<Nd> const& l) {
  point<Nd> v;
  v() = l.x_0() + 0.5 * (l.x_1() - l.x_0());
  return v;
}

}  // namespace geometry
}  // namespace hm3
