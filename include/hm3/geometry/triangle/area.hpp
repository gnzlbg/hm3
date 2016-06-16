#pragma once
/// \file
///
/// Triangle area
#include <hm3/geometry/polygon/area.hpp>
#include <hm3/geometry/triangle/triangle.hpp>
#include <hm3/geometry/triangle/vertices.hpp>

namespace hm3 {
namespace geometry {

// note: the area of a two-dimensional triangle is computed using the area
// algorithm for polygons

/// Area of a three-dimensional triangle
///
/// Uses Heron's formula: A = \sqrt{s(s - a)(s - b)(s - c)}
/// where s = p / 2 and p the perimeter of the triangle p = a + b + c.
num_t area(triangle<3> t) {
  auto v0 = vertex(t, 0);
  auto v1 = vertex(t, 1);
  auto v2 = vertex(t, 2);
  num_t a = (v0() - v1()).norm();
  num_t b = (v1() - v2()).norm();
  num_t c = (v2() - v0()).norm();

  num_t p = a + b + c;
  num_t s = .5 * p;
  return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

}  // namespace geometry
}  // namespace hm3
