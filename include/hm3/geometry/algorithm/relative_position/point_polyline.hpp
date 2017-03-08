#pragma once
/// \file
///
/// Relative position of point with respect to a polyline.
#include <hm3/geometry/algorithm/distance/minimum_segment_point.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/relative_position/point_segment.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/polyline.hpp>

namespace hm3::geometry {

namespace relative_position_point_polyline_detail {
struct relative_position_point_polyline_fn {
  template <typename P, typename PL>
  constexpr auto operator()(P const& p, PL const& polyline, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Polyline<PL, 2>{});
    num_t dist               = math::highest<num_t>;
    suint_t closest_edge_idx = 0;

    // Find the closest edge segment to the point
    for (auto&& edge_idx : edge.indices(polyline)) {
      auto&& edge_segment = edge(polyline, edge_idx);
      auto edge_dist
       = minimum_distance_segment_point(edge_segment, p, abs_tol, rel_tol);
      if (edge_dist < dist) {
        dist             = edge_dist;
        closest_edge_idx = edge_idx;
      }
    }
    HM3_ASSERT(!math::approx(dist, math::highest<num_t>), "no segment found?");
    return relative_position_point_segment(p, edge(polyline, closest_edge_idx));
  }
};

}  // namespace relative_position_point_polyline_detail

namespace {
static constexpr auto const& relative_position_point_polyline = static_const<
 with_default_tolerance<relative_position_point_polyline_detail::
                         relative_position_point_polyline_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
