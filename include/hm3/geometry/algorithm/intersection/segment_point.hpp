#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::segment_primitive {

/// Does the segment \p s intersect/contain the point \p p?
template <dim_t Nd>
inline bool intersection_test(segment<Nd> const& s,
                              point<Nd> const& p) noexcept {
  /// Compute the line parameter of the point
  const auto t_p_ = parameter(s, p);
  if (!t_p_) { return false; }  // point is not on the line => no intersection

  const num_t t_p = t_p_.value();

  // If the parameter is between zero and 1, the point is in the line segment
  if ((t_p > 0. or math::approx(t_p, 0.))
      and (t_p < 1. or math::approx(t_p, 1.))) {
    return true;
  }
  return false;
}

/// Intersection of a segment \p s with a point \p p is eitherhe point or
/// nothing.
template <dim_t Nd>
inline variant<monostate, point<Nd>> intersection(segment<Nd> const& s,
                                                  point<Nd> const& p) noexcept {
  if (intersection_test(s, p)) { return p; }
  return {};
}

}  // namespace hm3::geometry::segment_primitive
