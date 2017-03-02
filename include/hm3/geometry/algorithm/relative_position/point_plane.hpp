#pragma once
/// \file
///
/// Relative position of point with respect to a plane.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/concept/plane.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/relative_position.hpp>

namespace hm3::geometry {

namespace relative_position_point_plane_detail {
struct relative_position_point_plane_fn {
  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Plane<L, 2>{})>
  constexpr auto operator()(P const& p, L const& l) const noexcept {
    auto sd = l.signed_distance(p);

    if (approx_number(sd, 0)) {
      return relative_position_t::intersected;
    } else if (sd > 0.) {
      return relative_position_t::inside;
    } else {
      return relative_position_t::outside;
    }
  }
};

}  // namespace relative_position_point_plane_detail

namespace {
static constexpr auto const& relative_position_point_plane = static_const<
 relative_position_point_plane_detail::relative_position_point_plane_fn>::value;
}  // namespace

}  // namespace hm3::geometry
