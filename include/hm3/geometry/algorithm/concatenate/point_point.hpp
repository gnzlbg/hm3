#pragma once
/// \file
///
/// Concatenate two points into a segment.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace concatenate_point_point_detail {

struct concatenate_point_point_fn {
  /// Concatenates collinear adjacent points:
  template <typename P0, typename P1, typename UP0 = uncvref_t<P0>,
            typename UP1 = uncvref_t<P1>, typename R = associated::edge_t<UP0>>
  constexpr optional<R> operator()(P0&& p0, P1&& p1, num_t abs_tol,
                                   num_t rel_tol) const noexcept {
    static_assert(Point<UP0>{});
    static_assert(Point<UP1>{});
    static_assert(Same<UP0, UP1>{});
    if (approx_point(p0, p1, abs_tol, rel_tol)) { return {}; }
    using e_t = associated::edge_t<UP0>;
    return e_t{p0, p1};
  }
};

}  // namespace concatenate_point_point_detail

namespace {
constexpr auto const& concatenate_point_point
 = static_const<with_default_tolerance<
  concatenate_point_point_detail::concatenate_point_point_fn>>::value;
}

}  // namespace hm3::geometry
