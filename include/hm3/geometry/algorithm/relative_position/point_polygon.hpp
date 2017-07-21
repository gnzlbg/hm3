#pragma once
/// \file
///
/// Relative position of point with respect to a polygon.
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>

namespace hm3::geometry {

namespace relative_position_point_polygon_detail {
struct relative_position_point_polygon_fn {
  template <typename P, typename PG>
  constexpr auto operator()(P const& p, PG const& polygon, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Polygon<PG, 2>{});

    using intersection_test_polygon_point_detail::is_point_on_polygon_edge;
    using intersection_test_polygon_point_detail::is_inside;

    if constexpr (AABB<PG>{}) {
      if (not intersection_test_aabb_point(polygon, p, abs_tol, rel_tol)) {
        return relative_position_t::outside;
      }
      // The point is either inside or on the boundary.

      // If one of the point components is on the boundary, then the point
      // must be on the boundary:
      const auto min = x_min(polygon);
      const auto max = x_max(polygon);
      for (auto&& d : ambient_dimension[p]) {
        if (approx.eq(min(d), p(d), abs_tol, rel_tol)
            or approx.eq(max(d), p(d), abs_tol, rel_tol)) {
          return relative_position_t::intersected;
        }
      }

      // If its not on the boundary, then its completely inside:
      return relative_position_t::inside;
    } else {
      if (is_point_on_polygon_edge(p, polygon, abs_tol, rel_tol)) {
        return relative_position_t::intersected;
      }

      if (is_inside(p, polygon, abs_tol, rel_tol)) {
        return relative_position_t::inside;
      }
      return relative_position_t::outside;
    }
  }
};

}  // namespace relative_position_point_polygon_detail

namespace {
constexpr auto const& relative_position_point_polygon
 = static_const<with_default_tolerance<
  relative_position_point_polygon_detail::relative_position_point_polygon_fn>>::
  value;
}  // namespace

}  // namespace hm3::geometry
