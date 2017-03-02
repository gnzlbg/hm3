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
  template <typename T, typename U>
  constexpr bool operator()(T const& s, U const& p) const noexcept {
    static_assert(Point<U>{});
    static_assert(AABB<T>{});
    return approx.leq(x_min(s), p) and approx.geq(x_max(s), p);
  }
};

}  // intersection_test_aabb_point_detail

namespace {
static constexpr auto const& intersection_test_aabb_point = static_const<
 intersection_test_aabb_point_detail::intersection_test_aabb_point_fn>::value;
}

namespace intersection_aabb_point_detail {

struct intersection_aabb_point_fn {
  /// Intersection between the AABB \p s and the point \p p.
  template <typename T, typename U>
  constexpr variant<monostate, U> operator()(T const& s, U const& p) const
   noexcept {
    static_assert(Point<U>{});
    static_assert(AABB<T>{});
    if (intersection_test_aabb_point(s, p)) { return p; }
    return monostate{};
  }
};

}  // intersection_aabb_point_detail

namespace {
static constexpr auto const& intersection_aabb_point = static_const<
 intersection_aabb_point_detail::intersection_aabb_point_fn>::value;
}

}  // namespace hm3::geometry::aabb_primitive
