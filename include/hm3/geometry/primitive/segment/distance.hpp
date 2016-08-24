#pragma once
/// \file
///
/// Minimum distance between segment and points.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/primitive/line/parameter.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Minimum distance between the line \p l and the point \p p.
template <dim_t Nd>
num_t minimum_distance(segment<Nd> const& s, point<Nd> const& p) {
  // project point onto segment line
  auto sl = s.line();
  auto pp = point<Nd>(sl().projection(p()));

  // Is the projected point within the segment bounds?
  auto t   = parameter(sl, pp).value();
  auto tx1 = parameter(sl, s.x(1)).value();
  if (t >= 0. and t <= tx1) {
    // if distance == 0, point is on the line
    return distance.centroid(p, pp);
  }
  auto d0 = distance.centroid(s.x(0), p);
  auto d1 = distance.centroid(s.x(1), p);
  return std::min(d0, d1);
}

}  // namespace hm3::geometry::segment_primitive
