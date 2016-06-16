#pragma once
/// \file
///
/// Triangle centroid
#include <hm3/geometry/polygon/centroid.hpp>
#include <hm3/geometry/triangle/triangle.hpp>
#include <hm3/geometry/triangle/vertices.hpp>

namespace hm3 {
namespace geometry {

// note: the centroid of a two-dimensional triangle is computed using the
// centroid algorithm for polygons

/// Centroid of a three-dimensional triangle
point<3> centroid(triangle<3> t) {
  constexpr num_t f = 1. / 3.;

  auto v0 = vertex(t, 0);
  auto v1 = vertex(t, 1);
  auto v2 = vertex(t, 2);
  return point<3>(f * (v0() + v1() + v2()));
}

}  // namespace geometry
}  // namespace hm3
