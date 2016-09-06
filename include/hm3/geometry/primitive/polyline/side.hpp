#pragma once
/// \file
///
/// At which side of a polyline is a point located?
#include <hm3/geometry/algorithm/side.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polyline/faces.hpp>
#include <hm3/geometry/primitive/segment/distance.hpp>
#include <hm3/geometry/primitive/segment/side.hpp>

namespace hm3::geometry::polyline_primitive {

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Nd == 2 and Polyline<P, Nd>{})>
constexpr side_t side(P&& polyline, point<2> const& p) {
  num_t dist               = math::highest<num_t>;
  suint_t closest_face_idx = 0;

  // Find the closest face segment to the point
  for (auto&& face_idx : face_indices(polyline)) {
    auto&& face_segment = face(polyline, face_idx);
    auto face_dist      = distance.minimum(face_segment, p);
    if (face_dist < dist) {
      dist             = face_dist;
      closest_face_idx = face_idx;
    }
  }
  HM3_ASSERT(!math::approx(dist, math::highest<num_t>), "no segment found?");
  return side(face(polyline, closest_face_idx), p);
}

}  // namespace hm3::geometry::polyline_primitive
