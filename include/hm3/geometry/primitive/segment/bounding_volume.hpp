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
  using p_t = point<Nd>;
  return aabb<Nd>(p_t(s.x(0)().array().min(s.x(1)().array())),
                  p_t(s.x(0)().array().max(s.x(1)().array())));
}

/// Square Bounding Box of the segment \p s.
template <dim_t Nd>
constexpr box<Nd> square_bounding_box(segment<Nd> const& s) {
  return box<Nd>(centroid(s), (s.x(0)() - s.x(1)()).array().abs().maxCoeff());
}

}  // namespace hm3::geometry::segment_primitive
