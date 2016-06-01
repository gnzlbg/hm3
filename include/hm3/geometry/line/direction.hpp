#pragma once
/// \file
///
/// Line direction
#include <hm3/geometry/line/line.hpp>

namespace hm3 {
namespace geometry {

/// Line direction
template <dim_t Nd> constexpr point<Nd> direction(line<Nd> const& l) {
  point<Nd> v;
  v = (l.x_1() - l.x_0()) / (l.x_1() - l.x_0()).norm();
  return v;
}

/// Flip line direction
template <dim_t Nd> constexpr line<Nd> flip_direction(line<Nd> l) {
  using std::swap;
  swap(l.x_0, l.x_1);
  return l;
}

}  // namespace geometry
}  // namespace hm3
