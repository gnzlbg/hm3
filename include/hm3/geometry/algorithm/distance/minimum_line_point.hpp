#pragma once
/// \file
///
/// Minimum distance between a line and a point.
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/line.hpp>

namespace hm3::geometry {

namespace minimum_distance_line_point_detail {

struct minimum_distance_line_point_fn {
  /// Minimum distance between the line \p l and the point \p p.
  template <typename L, typename P, CONCEPT_REQUIRES_(Line<L>{} and Point<P>{})>
  constexpr auto operator()(L const& l, P const& p) const noexcept {
    return l().distance(p());
  }
};

} // namespace minimum_distance_line_point_detail

namespace {
static constexpr auto const &minimum_distance_line_point = static_const<
    minimum_distance_line_point_detail::minimum_distance_line_point_fn>::value;
}

} // namespace hm3::geometry
