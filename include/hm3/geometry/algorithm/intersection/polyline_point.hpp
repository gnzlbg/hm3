#pragma once
/// \file
///
/// Intersection of line polylines with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/primitive/polyline.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::polyline_primitive {

/// Does the polyline \p s intersect/contain the point \p p?
template <dim_t Nd, typename S>
inline bool intersection_test(polyline<Nd, S> const& s,
                              point<Nd> const& p) noexcept {
  return ranges::any_of(
   faces(s), [&](auto&& f) { return geometry::intersection.test(f, p); });
}

/// Intersection of a polyline \p s with a point \p p is eitherhe point or
/// nothing.
template <dim_t Nd, typename S>
inline variant<monostate, point<Nd>> intersection(polyline<Nd, S> const& s,
                                                  point<Nd> const& p) noexcept {
  if (intersection_test(s, p)) { return p; }
  return {};
}

}  // namespace hm3::geometry::polyline_primitive
