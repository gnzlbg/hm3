#pragma once
/// \file
///
/// Bounding Volumes of a line segment.
#include <hm3/geometry/discrete/algorithm/bounding_volume.hpp>
#include <hm3/geometry/discrete/primitive/aabb.hpp>
#include <hm3/geometry/discrete/primitive/box.hpp>
#include <hm3/geometry/discrete/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace segment_primitive {

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

}  // namespace segment_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
