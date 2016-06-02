#pragma once
/// \file
///
/// Line segment bounding box
#include <hm3/geometry/aabb.hpp>
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/segment/centroid.hpp>
#include <hm3/geometry/segment/segment.hpp>

namespace hm3 {
namespace geometry {

/// Axis-Aligned Bounding Box of the segment \p l
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(segment<Nd> const& l) {
  if (l.x_0 < l.x_1) { return aabb<Nd>(l.x_0, l.x_1); }
  return box<Nd>(l.x_1, l.x_0);
}

/// Square Bounding Box of the segment \p l
template <dim_t Nd> constexpr box<Nd> bounding_box(segment<Nd> const& l) {
  return box<Nd>(axis_aligned_bounding_box(l));
}

}  // namespace geometry
}  // namespace hm3
