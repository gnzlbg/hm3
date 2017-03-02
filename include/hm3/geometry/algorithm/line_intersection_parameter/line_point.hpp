#pragma once
/// \file
///
/// Parameter of the intersection of a point with a line.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/distance/minimum_line_point.hpp>
#include <hm3/utility/optional.hpp>

namespace hm3::geometry {

namespace intersection_parameter_line_point_detail {

template <typename T, typename U,
          CONCEPT_REQUIRES_((Line<T>{} or Segment<T>{}) and Point<U>{})>
constexpr num_t t(T const& l, U const& p, dim_t d, num_t abs_tol,
                  num_t rel_tol) noexcept {
  auto denom = l.x(1)(d) - l.x(0)(d);
  HM3_ASSERT(!approx_number(std::abs(denom), 0., abs_tol, rel_tol),
             "denom is zero (l: {}, p: {}, d: {})", l, p, d);
  return (p(d) - l.x(0)(d)) / denom;
}

struct intersection_parameter_line_point_fn {
  /// Line intersection parameter between the line \p l and the point \p p.
  ///
  /// Either value of the line parameter if the point \p lies in the line \p l
  /// or it returns nothing.
  template <typename L, typename P, CONCEPT_REQUIRES_(Line<L>{} and Point<P>{})>
  constexpr optional<num_t> operator()(L const& l, P const& p, num_t abs_tol,
                                       num_t rel_tol) const noexcept {
    constexpr auto ad = ambient_dimension(P{});
    const auto dist   = minimum_distance_line_point(l, p);
    if (approx_number(dist, 0., abs_tol, rel_tol)) {
      for (dim_t d = 0; d < ad; ++d) {
        if (!approx_number(l.direction()(d), 0., abs_tol, rel_tol)) {
          return t(l, p, d, abs_tol, rel_tol);
        }
      }
      HM3_ASSERT(false, "line: {}, direction vector has norm zero", l);
    }
    return {};
  }
};

}  // namespace intersection_parameter_line_point_detail

namespace {
static constexpr auto const& intersection_parameter_line_point = static_const<
 with_default_tolerance<intersection_parameter_line_point_detail::
                         intersection_parameter_line_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
