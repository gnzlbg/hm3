#pragma once
/// \file
///
/// Centroid of a line segment.
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Centroid coordinates of the line segment \p s.
template <dim_t Nd>
constexpr point<Nd> centroid(segment<Nd> const& s) noexcept {
  point<Nd> v;
  v() = s.x(0)() + 0.5 * (s.x(1)() - s.x(0)());
  return v;
}

}  // namespace hm3::geometry::segment_primitive
