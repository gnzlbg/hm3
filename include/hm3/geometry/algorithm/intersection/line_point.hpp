#pragma once
/// \file
///
/// Intersection of line with points.
#include <hm3/geometry/algorithm/line_intersection_parameter.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_line_point_detail {

struct intersection_test_line_point_fn {
  /// Does the point \p intersect the line \p l?
  template <typename L, typename P>
  constexpr bool operator()(L const& l, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Line<L>{});
    static_assert(Point<P>{});
    static_assert(ad_v<L> == ad_v<P>);
    if (line_intersection_parameter(l, p, abs_tol, rel_tol)) { return true; }
    return false;
  }
};

}  // namespace intersection_test_line_point_detail

namespace {
static constexpr auto const& intersection_test_line_point
 = static_const<with_default_tolerance<
  intersection_test_line_point_detail::intersection_test_line_point_fn>>::value;
}  // namespace

namespace intersection_line_point_detail {

struct intersection_line_point_fn {
  /// Does the point \p intersect the line \p l?

  template <typename L, typename P>
  constexpr variant<monostate, P> operator()(L const& l, P const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Line<L>{});
    static_assert(Point<P>{});
    static_assert(ad_v<L> == ad_v<P>);

    if (intersection_test_line_point(l, p, abs_tol, rel_tol)) { return p; }
    return monostate{};
  }
};

}  // namespace intersection_line_point_detail

namespace {
static constexpr auto const& intersection_line_point
 = static_const<with_default_tolerance<
  intersection_line_point_detail::intersection_line_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
