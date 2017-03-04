#pragma once
/// \file
///
/// Does the AABB intersect a point?
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/x_min_max.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry {

namespace intersection_test_aabb_point_detail {

struct intersection_test_aabb_point_fn {
  /// Does the AABB \p s and the point \p p intersect?
  template <typename A, typename P>
  constexpr bool operator()(A const& a, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Point<P>{});
    static_assert(AABB<A>{});
    static_assert(ad_v<A> == ad_v<P>);

    return approx.leq(x_min(a), p, abs_tol, rel_tol)
           and approx.geq(x_max(a), p, abs_tol, rel_tol);
  }
};

}  // intersection_test_aabb_point_detail

namespace {
static constexpr auto const& intersection_test_aabb_point
 = static_const<with_default_tolerance<
  intersection_test_aabb_point_detail::intersection_test_aabb_point_fn>>::value;
}

namespace intersection_aabb_point_detail {

struct intersection_aabb_point_fn {
  /// Intersection between the AABB \p s and the point \p p.
  template <typename A, typename P>
  constexpr variant<monostate, P> operator()(A const& a, P const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Point<P>{});
    static_assert(AABB<A>{});
    static_assert(ad_v<A> == ad_v<P>);
    if (intersection_test_aabb_point(a, p, abs_tol, rel_tol)) { return p; }
    return monostate{};
  }
};

}  // intersection_aabb_point_detail

namespace {
static constexpr auto const& intersection_aabb_point
 = static_const<with_default_tolerance<
  intersection_aabb_point_detail::intersection_aabb_point_fn>>::value;
}

}  // namespace hm3::geometry::aabb_primitive
