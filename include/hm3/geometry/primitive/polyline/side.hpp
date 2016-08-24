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
constexpr side_t side(P&& l, point<2> const& p) {
  num_t d        = math::highest<num_t>;
  suint_t sg_idx = 0;

  // find the segment with the smallest distance to the point
  for (auto&& sidx : face_indices(l)) {
    auto&& s = face(l, sidx);
    auto sd  = distance.minimum(s, p);
    if (sd < d) {
      d      = sd;
      sg_idx = sidx;
    }
  }
  HM3_ASSERT(!math::approx(d, math::highest<num_t>), "no segment found?");
  return side(face(l, sg_idx), p);
}

}  // namespace hm3::geometry::polyline_primitive
