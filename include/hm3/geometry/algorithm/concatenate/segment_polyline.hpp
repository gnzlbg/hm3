#pragma once
/// \file
///
/// Concatenate a segment and a polyline.
#include <hm3/geometry/algorithm/concatenate/polyline_polyline.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace concatenate_segment_polyline_detail {

struct concatenate_segment_polyline_fn {
  /// Set union between a segment and a polyline
  template <typename S, typename P, typename US = uncvref_t<S>,
            typename UP = uncvref_t<P>>
  constexpr optional<UP> operator()(S&& s, P&& l, num_t abs_tol,
                                    num_t rel_tol) const noexcept {
    static_assert(Segment<US>{});
    static_assert(MutablePolyline<UP>{});
    static_assert(Same<associated::edge_t<UP>, US>{});
    return concatenate_polyline_polyline(UP({s}), l, abs_tol, rel_tol);
  }
};

}  // namespace concatenate_segment_polyline_detail

namespace {
constexpr auto const& concatenate_segment_polyline
 = static_const<with_default_tolerance<
  concatenate_segment_polyline_detail::concatenate_segment_polyline_fn>>::value;
}

}  // namespace hm3::geometry
