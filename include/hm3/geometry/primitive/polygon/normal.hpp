#pragma once
/// \file
///
/// Normal vector of a polygon.
#include <hm3/geometry/algorithm/normal.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/polygon/order.hpp>
#include <hm3/geometry/primitive/polygon/planar.hpp>

namespace hm3::geometry::polygon_primitive {

template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr vec<3> normal(P&& p) noexcept {
  if (vertex_order.counter_clockwise(p)) { return vec<3>::unit(2); }
  return vec<3>({0., 0., -1});
}

template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr vec<3> normal(P&& p) noexcept {
  HM3_ASSERT(planar.is(p),
             "non-planar 3D polygon don't have a plane normal vector");

  // Get 3 non-colinear vertices (colinearity is asserted in planar.is(p)):
  auto v0 = vertex(p, 0);
  auto v1 = vertex(p, 1);
  auto vN = vertex(p, vertex_size(p) - 1);

  // The normal is orthogonal to two vectors in the polygon's plane:
  return (v1() - v0()).cross(vN() - v0()).normalized();
}

}  // namepsace hm3::geometry::polygon_primitive
