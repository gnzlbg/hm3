#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::polygon_primitive {

/// Does the segment \p s intersect/contain the point \p p?
template <typename P, dim_t Nd = dimension_v<P>,
          CONCEPT_REQUIRES_(Polygon<P, Nd>{})>
inline bool intersection_test(P const& s, point<Nd> const& p) noexcept {
  static_assert(Nd > 1, "");
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
