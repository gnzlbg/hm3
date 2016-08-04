#pragma once
/// \file
///
/// Split line segment at a point.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3 {
namespace geometry {

namespace discrete {
namespace segment_primitive {

template <dim_t Nd>  //
struct segment_point_split_result {
  inline_vector<segment<Nd>, 2> segments;
};

/// Splits the segment \p s at the point \p p.
template <dim_t Nd>
inline segment_point_split_result<Nd> split(segment<Nd> const& s,
                                            point<Nd> const& p) {
  segment_point_split_result<Nd> result;
  if (!intersection_test(s, p)) { return result; }

  // If the point is a segment end point, the result is just the segment:
  if (a.x(0) == p or a.x(1) == p) {
    result.segments.push_back(s);
    return result;
  }

  result.segments.push_back(segment<Nd>::through(a.x(0), p));
  result.segments.push_back(segment<Nd>::through(p, a.x(1)));
  return result;
}

}  // namespace segment_primitive
}  // namespace discrete

}  // namespace geometry
}  // namespace hm3
