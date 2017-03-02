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
  template <typename P, typename L,
            CONCEPT_REQUIRES_(Point<P, 2>{} and Line<L, 2>{})>
  constexpr auto operator()(P const& p, L const& l) const noexcept {
    auto a         = l.origin();
    auto ab        = l.direction();
    using vector_t = uncvref_t<decltype(ab)>;
    auto ap        = vector_t(a() - p());

    auto d = perp_product(ab, ap);

    if (d > 0.) {
      return relative_position_t::outside;
    } else if (d < 0.) {
      return relative_position_t::inside;
    }
    HM3_ASSERT(math::approx(d, 0.), "");
    return relative_position_t::intersected;
  }
};

}  // namespace relative_position_point_line_detail

namespace {
static constexpr auto const& relative_position_point_line = static_const<
 relative_position_point_line_detail::relative_position_point_line_fn>::value;
}  // namespace

}  // namespace hm3::geometry
