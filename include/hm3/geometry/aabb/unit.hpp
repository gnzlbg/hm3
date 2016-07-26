#pragma once
/// \file
///
/// Unit AABB
#include <hm3/geometry/aabb/concept.hpp>
#include <hm3/geometry/point.hpp>

namespace hm3 {
namespace geometry {

namespace aabb_primitive {

/// Constructs an unit AABB [0., 1.]
template <typename Shape, CONCEPT_REQUIRES_(AABB<Shape>{})>
auto unit(Shape) noexcept {
  static constexpr dim_t nd = dimension(Shape{});
  return Shape{point<nd>::constant(0.), point<nd>::constant(1.)};
}

}  // namespace aabb_primitive

}  // namespace geometry
}  // namespace hm3
