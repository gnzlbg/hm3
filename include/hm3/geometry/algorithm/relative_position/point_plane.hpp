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
  template <typename P, typename L>
  constexpr auto operator()(P const& p, L const& l, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P>{});
    static_assert(Plane<L>{});
    static_assert(ad_v<P> == ad_v<L>);
    auto sd = l.signed_distance(p);

    if (approx_number(sd, 0, abs_tol, rel_tol)) {
      return relative_position_t::intersected;
    } else if (sd > 0.) {
      return relative_position_t::inside;
    }
    return relative_position_t::outside;
  }
};

}  // namespace relative_position_point_plane_detail

namespace {
constexpr auto const& relative_position_point_plane
 = static_const<with_default_tolerance<
  relative_position_point_plane_detail::relative_position_point_plane_fn>>::
  value;
}  // namespace

}  // namespace hm3::geometry
