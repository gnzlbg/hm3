#pragma once
/// \file
///
/// Line bounding box
#include <hm3/geometry/aabb.hpp>
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/line/centroid.hpp>
#include <hm3/geometry/line/line.hpp>

namespace hm3 {
namespace geometry {

/// Square Bounding Box of the line \p l
template <dim_t Nd> constexpr box<Nd> bounding_box(line<Nd> const& l) {
  if (l.x_0 < l.x_1) { return box<Nd>(l.x_0, l.x_1); }
  return box<Nd>(l.x_1, l.x_0);
}

/// Axis-Aligned Bounding Box of the line \p l
template <dim_t Nd>
constexpr aabb<Nd> axis_aligned_bounding_box(line<Nd> const& l) {
  return aabb<Nd>(l.x_0, l.x_1);
}

}  // namespace geometry
}  // namespace hm3
