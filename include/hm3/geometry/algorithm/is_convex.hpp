#pragma once
/// \file
///
/// Is the polygon convex?
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/algorithm/bounding_length/aabb.hpp>
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/algorithm/planar_projection.hpp>
#include <hm3/geometry/concept/aabb.hpp>

namespace hm3::geometry {

namespace is_convex_detail {

struct is_convex_fn {
  /// This computes the perp product of the direction vectors of all pairs of
  /// adjacent edges.
  ///
  /// If the sign of the perp product is positive or negative for all pairs of
  /// adjacent edges, then the polygon is convex. Otherwise the polygon is
  /// non-convex.
  template <typename P>
  static constexpr bool is_convex_2d(P&& p) noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    auto no_edges = edge_size(p);

    auto perp_product_sign = [&](auto&& e0, auto&& e1) {
      return math::sign(
       perp_product(direction(edge(p, e0)), direction(edge(p, e1))));
    };
    const auto pps = perp_product_sign(0, 1);
    for (dim_t e = 1; e < no_edges - 1; ++e) {
      if (pps != perp_product_sign(e, e + 1)) { return false; };
    }
    return pps != perp_product_sign(no_edges - 1, 0) ? false : true;
  }

  template <typename P>
  constexpr bool is_convex_3d(P&& p) const noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    static_assert(ad_v<P> == 3);
    auto pp = planar_projection.to(p);
    return is_convex_2d(pp);
  }

  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
  constexpr bool operator()(P&& p) const noexcept {
    if
      constexpr(AABB<P, 2>{}) { return true; }
    else {
      return is_convex_2d(std::forward<P>(p));
    }
  }

  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
  constexpr bool operator()(P&& p) const noexcept {
    if
      constexpr(AABB<P, 2>{}) { return true; }
    else {
      return is_convex_3d(std::forward<P>(p));
    }
  }
};

}  // namespace is_convex_detail

namespace {
constexpr auto const& is_convex
 = static_const<with_default_tolerance<is_convex_detail::is_convex_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
