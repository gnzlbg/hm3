#pragma once
/// \file
///
/// Bounding Volumes of a line segment.
#include <hm3/geometry/algorithm/bounding_volume.hpp>
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Axis-Aligned Bounding Box of the segment \p s.
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(segment<Nd> const& s) {
  if (s.x(0) < s.x(1)) { return aabb<Nd>(s.x(0), s.x(1)); }
  return aabb<Nd>(s.x(1), s.x(0));
}

/// Square Bounding Box of the segment \p s.
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(segment<Nd> const& s) {
  return box<Nd>(centroid(s), (s.x(0)() - s.x(1)()).array().abs().maxCoeff());
}

}  // namespace hm3::geometry::segment_primitive
