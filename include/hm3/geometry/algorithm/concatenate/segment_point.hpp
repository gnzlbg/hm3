#pragma once
/// \file
///
/// Concatenate a segment and a point.
#include <hm3/geometry/algorithm/concatenate/segment_segment.hpp>

namespace hm3::geometry {

namespace concatenate_segment_point_detail {

struct concatenate_segment_point_fn {
  /// Concatenates segment with a point:
  template <typename S, typename P, typename US = uncvref_t<S>,
            typename UP = uncvref_t<P>,
            typename R  = associated::concatenated_t<associated::edge_t<US>>>
  constexpr optional<R> operator()(S&& s, P&& p, num_t abs_tol,
                                   num_t rel_tol) const noexcept {
    static_assert(Segment<US>{});
    static_assert(Point<UP>{});
    static_assert(ad_v<US> == ad_v<UP>);
    using e_t = associated::edge_t<UP>;
    static_assert(Same<US, e_t>{});
    // Create an adjacent segment and concatenate the two segments
    auto s2 = e_t{s.x(1), p};
    return concatenate_segment_segment(s, s2, abs_tol, rel_tol);
  }
};

}  // namespace concatenate_segment_point_detail

namespace {
constexpr auto const& concatenate_segment_point
 = static_const<with_default_tolerance<
  concatenate_segment_point_detail::concatenate_segment_point_fn>>::value;
}

}  // namespace hm3::geometry
