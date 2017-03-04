#pragma once
/// \file
///
/// Minimum distance between a segment and a point.
#include <hm3/geometry/algorithm/distance/centroid.hpp>
#include <hm3/geometry/algorithm/line_intersection_parameter/line_point.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace minimum_distance_segment_point_detail {

struct minimum_distance_segment_point_fn {
  /// Minimum distance between the segment \p s and the point \p p.
  template <typename S, typename P,
            CONCEPT_REQUIRES_(Segment<S>{} and Point<P>{})>
  constexpr auto operator()(S const& s, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    // project point onto segment line
    auto sl = s.line();
    auto pp = P(sl().projection(p()));

    // Is the projected point within the segment bounds?
    auto t_ = intersection_parameter_line_point(sl, pp, abs_tol, rel_tol);
    HM3_ASSERT(t_,
               "projecting point: {} onto segment line: {} of segment: {} "
               "returns: {} which does not lie on the line",
               p, sl, s, pp);
    auto t    = t_.value();
    auto tx1_ = intersection_parameter_line_point(sl, s.x(1), abs_tol, rel_tol);
    HM3_ASSERT(
     tx1_,
     "segment x(1) end: {} of segment: {} does not lie on the segment line: {}",
     s.x(1), s, sl);
    auto tx1 = tx1_.value();
    if (approx.geq(t, 0., abs_tol, rel_tol)
        and approx.leq(t, tx1, abs_tol, rel_tol)) {
      // if distance == 0, point is on the line
      return distance_centroid(p, pp);
    }
    auto d0 = distance_centroid(s.x(0), p);
    auto d1 = distance_centroid(s.x(1), p);
    return std::min(d0, d1);
  }
};

}  // namespace minimum_distance_segment_point_detail

namespace {
static constexpr auto const& minimum_distance_segment_point
 = static_const<with_default_tolerance<
  minimum_distance_segment_point_detail::minimum_distance_segment_point_fn>>::
  value;
}

}  // namespace hm3::geometry
