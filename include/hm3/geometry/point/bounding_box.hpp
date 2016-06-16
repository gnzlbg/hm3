#pragma once
/// \file
///
/// Bounding box of a point
#include <hm3/geometry/aabb.hpp>
#include <hm3/geometry/box/box.hpp>
#include <hm3/geometry/point/point.hpp>

namespace hm3 {
namespace geometry {

/// Square Bounding Box of a point
template <dim_t Nd>  //
constexpr geometry::box<Nd> square_bounding_box(point<Nd> p) noexcept {
  return geometry::box<Nd>{p, math::eps};
}

/// Axis-Aligned Bounding Box of a point (it is just a box)
template <dim_t Nd>  //
constexpr geometry::box<Nd> axis_aligned_bounding_box(point<Nd> p) noexcept {
  return square_bounding_box(p);
}

}  // namespace geometry
}  // namespace hm3
