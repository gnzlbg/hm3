#pragma once
/// \file
///
/// Intersection between two points.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_point_point_detail {

struct intersection_test_point_point_fn {
  template <typename T>
  constexpr bool operator()(T const& a, T const& b, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<T>{});
    return approx_point(a, b, abs_tol, rel_tol);
  }
};

}  // namespace intersection_test_point_point_detail

namespace {
static constexpr auto const& intersection_test_point_point
 = static_const<with_default_tolerance<
  intersection_test_point_point_detail::intersection_test_point_point_fn>>::
  value;
}  // namespace

namespace intersection_point_point_detail {

struct intersection_point_point_fn {
  template <typename T>
  constexpr variant<monostate, T> operator()(T const& a, T const& b,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Point<T>{});
    if (intersection_test_point_point(a, b, abs_tol, rel_tol)) { return a; }
    return {};
  }
};

}  // namespace intersection_point_point_detail

namespace {
static constexpr auto const& intersection_point_point
 = static_const<with_default_tolerance<
  intersection_point_point_detail::intersection_point_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
