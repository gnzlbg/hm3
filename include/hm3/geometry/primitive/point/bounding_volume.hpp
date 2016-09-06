#pragma once
/// \file
///
/// Bounding box of a point
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/aabb/aabb.hpp>
#include <hm3/geometry/primitive/box/bounding_volume.hpp>
#include <hm3/geometry/primitive/box/box.hpp>
#include <hm3/geometry/primitive/fwd.hpp>
#include <hm3/geometry/primitive/point/point.hpp>

namespace hm3::geometry::point_primitive {

/// Square Bounding Box of point \p p.
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(point<Nd> const& p) noexcept {
  return box<Nd>{p, 2. * math::eps};
}

/// Axis-Aligned Bounding Box of point \p p (it is just a box).
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(point<Nd> const& p) noexcept {
  return bounding_volume.aabb(square_bounding_box(p));
}

}  // namespace hm3::geometry::point_primitive
