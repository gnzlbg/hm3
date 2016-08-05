#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::segment_primitive {

/// Does the segment \p s intersect/contain the point \p p?
template <dim_t Nd>
inline bool intersection_test(segment<Nd> const& s,
                              point<Nd> const& p) noexcept {
  auto line = s();
  // If the distance of the point to the line is not zero, no intersection:
  if (!math::approx(line.squaredDistance(p()), 0.)) { return false; }
  // The point is on the line of the segment:
  num_t t_p = (p(0) - s.x(0)(0)) / (s.x(1)(0) - s.x(0)(0));

  HM3_ASSERT(
   [&]() {
     for (dim_t d = 1; d < Nd; ++d) {
       HM3_ASSERT(math::approx(s.x(0)(d) + t_p * (s.x(1)(d) - s.x(0)(d)), p(d)),
                  "");
     }
     return true;
   }(),
   "");

  if (t_p >= 0. and t_p <= 1.) { return true; }
  return false;
}

/// Intersection of a segment \p s with a point \p p is eitherhe point or
/// nothing.
template <dim_t Nd>
inline optional<point<Nd>> intersection(segment<Nd> const& s,
                                        point<Nd> const& p) noexcept {
  if (intersection_test(s, p)) { return optional<point<Nd>>{p}; }
  return optional<point<Nd>>{};
}

}  // namespace hm3::geometry::segment_primitive
