#pragma once
/// \file
///
/// Relative position of point with respect to a plane.
#include <hm3/geometry/algorithm/relative_position/point_plane.hpp>
#include <hm3/geometry/concept/aabb.hpp>

namespace hm3::geometry {

namespace relative_position_aabb_plane_detail {
struct relative_position_aabb_plane_fn {
  template <typename P, typename L>
  constexpr auto operator()(P const& b, L const& l, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(AABB<P>{});
    static_assert(Plane<L>{});
    static_assert(ad_v<P> == ad_v<L>);
    auto rp = relative_position_point_plane(vertex(b, 0), l, abs_tol, rel_tol);
    for (dim_t vx = 1, e = vertex_size(b); vx != e; ++vx) {
      if (rp
          != relative_position_point_plane(vertex(b, vx), l, abs_tol,
                                           rel_tol)) {
        return relative_position_t::intersected;
      }
    }
    return rp;
  }
};

}  // namespace relative_position_aabb_plane_detail

namespace {
constexpr auto const& relative_position_aabb_plane
 = static_const<with_default_tolerance<
  relative_position_aabb_plane_detail::relative_position_aabb_plane_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
