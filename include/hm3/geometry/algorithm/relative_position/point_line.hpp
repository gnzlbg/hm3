#pragma once
/// \file
///
/// Relative position of point with respect to a line.
#include <hm3/geometry/algorithm/cross.hpp>
#include <hm3/geometry/concept/line.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/relative_position.hpp>

namespace hm3::geometry {

namespace relative_position_point_line_detail {
struct relative_position_point_line_fn {
  template <typename P, typename L>
  constexpr auto operator()(P const& p, L const& l, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P, 2>{});
    static_assert(Line<L, 2>{});
    auto a         = l.origin();
    auto ab        = l.direction();
    using vector_t = uncvref_t<decltype(ab)>;
    auto ap        = vector_t(a() - p());

    auto d = perp_product(ab, ap);

    if (approx_number(d, 0., abs_tol, rel_tol)) {
      return relative_position_t::intersected;
    }

    if (d > 0.) { return relative_position_t::outside; }
    return relative_position_t::inside;
  }
};

}  // namespace relative_position_point_line_detail

namespace {
static constexpr auto const& relative_position_point_line
 = static_const<with_default_tolerance<
  relative_position_point_line_detail::relative_position_point_line_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
