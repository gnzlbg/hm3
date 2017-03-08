#pragma once
/// \file
///
/// Intersection of ray with points.
#include <hm3/geometry/algorithm/line_intersection_parameter.hpp>
#include <hm3/ext/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_ray_point_detail {

struct intersection_test_ray_point_fn {
  /// Does the point \p intersect the ray \p l?
  template <typename L, typename P>
  constexpr bool operator()(L const& l, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Ray<L>{});
    static_assert(Point<P>{});
    static_assert(ad_v<L> == ad_v<L>);

    if (auto t = line_intersection_parameter(l.line(), p, abs_tol, rel_tol)) {
      return approx.geq(t.value(), 0., abs_tol, rel_tol);
    }
    return false;
  }
};

}  // namespace intersection_test_ray_point_detail

namespace {
static constexpr auto const& intersection_test_ray_point
 = static_const<with_default_tolerance<
  intersection_test_ray_point_detail::intersection_test_ray_point_fn>>::value;
}  // namespace

namespace intersection_test_non_degenerate_ray_point_detail {

struct intersection_test_non_degenerate_ray_point_fn {
  /// Does the point \p intersect the ray \p l?
  template <typename L, typename P>
  constexpr non_degenerate_intersection_test_result operator()(
   L const& l, P const& p, num_t abs_tol, num_t rel_tol) const noexcept {
    static_assert(Ray<L>{});
    static_assert(Point<P>{});
    static_assert(ad_v<L> == ad_v<L>);

    if (intersection_test(l, p, abs_tol, rel_tol)) {
      return non_degenerate_intersection_test_result::intersection;
    }
    return non_degenerate_intersection_test_result::no_intersection;
  }
};

}  // namespace intersection_test_non_degenerate_ray_point_detail

namespace {
static constexpr auto const& intersection_test_non_degenerate_ray_point
 = static_const<with_default_tolerance<
  intersection_test_non_degenerate_ray_point_detail::
   intersection_test_non_degenerate_ray_point_fn>>::value;
}  // namespace

namespace intersection_ray_point_detail {

struct intersection_ray_point_fn {
  /// Does the point \p intersect the ray \p l?

  template <typename L, typename P>
  constexpr variant<monostate, P> operator()(L const& l, P const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Ray<L>{});
    static_assert(Point<P>{});
    static_assert(ad_v<L> == ad_v<L>);

    if (intersection_test_ray_point(l, p, abs_tol, rel_tol)) { return p; }
    return monostate{};
  }
};

}  // namespace intersection_ray_point_detail

namespace {
static constexpr auto const& intersection_ray_point
 = static_const<with_default_tolerance<
  intersection_ray_point_detail::intersection_ray_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
