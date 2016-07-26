#pragma once
/// \file
///
/// Does the AABB contains the point?
#include <hm3/geometry/aabb/concept.hpp>
namespace hm3 {
namespace geometry {

namespace aabb_primitive {

/// Does the AABB \p s contains the point \p p?
template <typename Shape, typename Point, CONCEPT_REQUIRES_(AABB<Shape>{})>
constexpr bool contains(Shape const& s, Point const& p) noexcept {
  const auto b = bounds(s);
  return b.min < p and b.max > p;
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
