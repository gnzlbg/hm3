#pragma once
/// \file
///
/// Does the rectangle contains the point?
#include <hm3/geometry/rectangle/concept.hpp>
namespace hm3 {
namespace geometry {

/// Contains point
template <typename Shape, typename Point, CONCEPT_REQUIRES_(Rectangle<Shape>{})>
constexpr bool contains(Shape const& s, Point&& p) noexcept {
  const auto b = bounds(s);
  return b.x_min() < p() and b.x_max() > p();
}

}  // namespace geometry
}  // namespace hm3
