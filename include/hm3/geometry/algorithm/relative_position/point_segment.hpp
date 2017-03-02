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
  template <typename P, typename S,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Segment<S, 2>{})>
  constexpr auto operator()(P const& p, S const& s) const noexcept {
    auto l = s.line();
    return relative_position_point_line(p, l);
  }
};

}  // namespace relative_position_point_segment_detail

namespace {
static constexpr auto const& relative_position_point_segment
 = static_const<relative_position_point_segment_detail::
                 relative_position_point_segment_fn>::value;
}  // namespace

}  // namespace hm3::geometry
