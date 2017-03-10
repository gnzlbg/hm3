#pragma once
/// \file
///
/// Relative position of point with respect to a segment.
#include <hm3/geometry/algorithm/relative_position/point_line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/segment.hpp>

namespace hm3::geometry {

namespace relative_position_point_segment_detail {
struct relative_position_point_segment_fn {
  template <typename P, typename S>
  constexpr auto operator()(P const& p, S const& s, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Segment<S, 2>{});
    auto l = s.line();
    return relative_position_point_line(p, l, abs_tol, rel_tol);
  }
};

}  // namespace relative_position_point_segment_detail

namespace {
constexpr auto const& relative_position_point_segment
 = static_const<with_default_tolerance<
  relative_position_point_segment_detail::relative_position_point_segment_fn>>::
  value;
}  // namespace

}  // namespace hm3::geometry
