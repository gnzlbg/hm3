#pragma once
/// \file
///
/// Computes the order of the vertices of a polygon (clockwise or counter
/// clockwise).
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry {

namespace polygon_primitive {

namespace order_detail {

/// Returns true if the xy components of the vertices of the polygon \p p
/// are
/// sorted in counter clock-wise (ccw) order, false otherwise.
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr bool counter_clockwise_2d(P&& p) {
  auto&& vs    = vertices(p);
  const auto e = size(vs) - 1;
  num_t tmp    = 0.;
  for (dim_t i = 0; i < e; ++i) {
    tmp += (vs[i + 1](0) - vs[i](0)) * (vs[i + 1](1) + vs[i](1));
  }
  {  // wrap around
    tmp += (vs[0](0) - vs[e](0)) * (vs[0](1) + vs[e](1));
  }
  return tmp < 0;
}

struct vertex_order_fn {
  /// Returns true if the vertices of the polygon \p p are sorted in counter
  /// clock-wise (ccw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
  static constexpr bool counter_clockwise(P&& p) {
    return counter_clockwise_2d(std::forward<P>(p));
  }

  // Counter-cloclwise doesn't make sense in 3D:
  /// Returns true if the vertices of the polygon \p p are sorted in counter
  /// clock-wise (ccw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
  static constexpr bool counter_clockwise(P&& p) {
    return counter_clockwise_2d(planar.to(std::forward<P>(p)));
  }

  /// Returns true if the vertices of the polygon \p p are sorted in
  /// clock-wise (cw) order, false otherwise.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
  static constexpr bool clockwise(P&& p) {
    return !counter_clockwise(std::forward<P>(p));
  }
};

}  // namespace order_detail

namespace {
constexpr auto const& vertex_order
 = static_const<order_detail::vertex_order_fn>::value;
}  // namespace

}  // namespace polygon_primitive

using polygon_primitive::vertex_order;

}  // namespace hm3::geometry
