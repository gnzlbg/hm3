#pragma once
/// \file
///
/// Bounding box of a point
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/box/box.hpp>
#include <hm3/geometry/discrete/primitive/fwd.hpp>
#include <hm3/geometry/discrete/primitive/point/point.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {

namespace box_primitive {
template <dim_t Nd> struct box;
}  // namespace box_primitive

using box_primitive::box;

namespace point_primitive {

/// Square Bounding Box of point \p p.
template <dim_t Nd>  //
constexpr box<Nd> square_bounding_box(point<Nd> p) noexcept {
  return box<Nd>{p, math::eps};
}

/// Axis-Aligned Bounding Box of point \p p (it is just a box).
template <dim_t Nd>  //
constexpr box<Nd> axis_aligned_bounding_box(point<Nd> p) noexcept {
  return square_bounding_box(p);
}

}  // namespace point_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
