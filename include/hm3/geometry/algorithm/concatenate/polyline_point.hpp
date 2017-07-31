#pragma once
/// \file
///
/// Concatenate a polyline and a point.
#include <hm3/geometry/algorithm/concatenate/polyline_polyline.hpp>

namespace hm3::geometry {

namespace concatenate_polyline_point_detail {

struct concatenate_polyline_point_fn {
  /// Concatenates polyline with a point:
  template <typename PL, typename P, typename UPL = uncvref_t<PL>,
            typename UP = uncvref_t<P>,
            typename R  = associated::concatenated_t<associated::edge_t<UP>>>
  constexpr optional<R> operator()(PL&& pl, P&& p, num_t abs_tol,
                                   num_t rel_tol) const noexcept {
    static_assert(Polyline<UPL>{});
    static_assert(Point<UP>{});
    static_assert(ad_v<UPL> == ad_v<UP>);
    using e_t = associated::edge_t<UP>;
    static_assert(Same<associated::edge_t<UPL>, e_t>{});
    static_assert(Same<R, UPL>{});
    // Create an adjacent segment and concatenate it with the polyline
    auto s = R{view::single(e_t{vertex.last(pl), p})};
    return concatenate_polyline_polyline(pl, s, abs_tol, rel_tol);
  }
};

}  // namespace concatenate_polyline_point_detail

namespace {
constexpr auto const& concatenate_polyline_point
 = static_const<with_default_tolerance<
  concatenate_polyline_point_detail::concatenate_polyline_point_fn>>::value;
}

}  // namespace hm3::geometry
