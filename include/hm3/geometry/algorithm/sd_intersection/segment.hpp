#pragma once
/// \file
///
/// Intersection of line segments with signed distance fields.
#include <hm3/geometry/algorithm/sd_intersection.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/geometry/sd/concepts.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::segment_primitive {

/// Intersection of the line segment \p s with the zero-th level of the signed
/// distance field \p sdf .
template <dim_t Nd, typename SD, CONCEPT_REQUIRES_(SignedDistance<SD, Nd>{})>
constexpr variant<monostate, point<Nd>, segment<Nd>>
 signed_distance_intersection(segment<Nd> const& s, SD&& sdf) {
  array<num_t, 2> sdv{sdf(s.x(0)), sdf(s.x(1))};
  array<sint_t, 2> sgv{{math::signum(sdv[0]), math::signum(sdv[1])}};

  switch (sd_intersection.test(sgv)) {
    case intersected: {
      if (sgv[0] == sgv[1]) {
        HM3_ASSERT(sgv[0] == 0, "");
        return s;
      }
      return point<Nd>{}; //linear_interpolation::point_at(s, sdv, 0.);
    }
    case inside:
    case outside: {
      return monostate{};
    }
  }
}

}  // namespace hm3::geometry::segment_primitive
