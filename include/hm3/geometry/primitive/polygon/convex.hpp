#pragma once
/// \file
///
/// Is a polygon convex?
#include <hm3/geometry/primitive/polygon/planar.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

namespace convex_detail {

struct convex_fn {
  /// This computes the perp product of the direction vectors of all pairs of
  /// adjacent edges.
  ///
  /// If the sign of the perp product is positive or negative for all pairs of
  /// adjacent edges, then the polygon is convex. Otherwise the polygon is
  /// non-convex.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  static constexpr bool convex_2d(P&& p) noexcept {
    auto no_faces = face_size(p);

    auto perp_product_sign = [&](auto&& f0, auto&& f1) {
      return math::sign(
       perp_product(direction(face(p, f0)), direction(face(p, f1))));
    };

    const auto pps = perp_product_sign(0, 1);
    for (dim_t f = 1; f < no_faces - 1; ++f) {
      if (pps != perp_product_sign(f, f + 1)) { return false; };
    }
    return pps != perp_product_sign(no_faces - 1, 0) ? false : true;
  }

  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
  constexpr bool operator()(P&& p) const noexcept {
    return convex_2d(std::forward<P>(p));
  }

  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
  constexpr bool operator()(P&& p) const noexcept {
    auto b  = planar.basis(p);
    auto pp = planar.to(p, b);
    return convex_2d(pp);
  }
};

}  // namespace convex_detail

namespace {
constexpr auto const& convex = static_const<convex_detail::convex_fn>::value;
}  // namespace

}  // polygon_primitive

using polygon_primitive::convex;

}  // namespace hm3::geometry
