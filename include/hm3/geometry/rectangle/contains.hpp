#pragma once
/// \file
///
/// Does the rectangle contains the point?
/// TODO: add tests
#include <hm3/geometry/rectangle/concept.hpp>
namespace hm3 {
namespace geometry {

/// Contains point
template <typename Shape, typename Point, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr bool contains(Shape const& s, Point const& p) noexcept {
  const auto b = bounds(s);
  return b.min < p and b.max > p;
}

}  // namespace geometry
}  // namespace hm3
