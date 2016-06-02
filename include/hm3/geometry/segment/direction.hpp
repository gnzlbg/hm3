#pragma once
/// \file
///
/// Line segment direction
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Line direction
template <dim_t Nd> constexpr point<Nd> direction(segment<Nd> const& l) {
  point<Nd> v;
  v = (l.x_1() - l.x_0()) / (l.x_1() - l.x_0()).norm();
  return v;
}

/// Flip line segment direction
template <dim_t Nd> constexpr segment<Nd> flip_direction(segment<Nd> l) {
  using std::swap;
  swap(l.x_0, l.x_1);
  return l;
}

}  // namespace geometry
}  // namespace hm3
