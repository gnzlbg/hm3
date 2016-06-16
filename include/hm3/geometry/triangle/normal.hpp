#pragma once
/// \file
///
/// Triangle normal
#include <hm3/geometry/triangle/triangle.hpp>
#include <hm3/geometry/triangle/vertices.hpp>

namespace hm3 {
namespace geometry {

/// Normal vector of a three-dimensional triangle
vector<3> normal(triangle<3> t) {
  auto v0 = vertex(t, 0);
  auto v1 = vertex(t, 1);
  auto v2 = vertex(t, 2);

  return (v1() - v0()).cross(v2() - v0()).normalized();
}

}  // namespace geometry
}  // namespace hm3
