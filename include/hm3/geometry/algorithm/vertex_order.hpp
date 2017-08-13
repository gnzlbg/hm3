#pragma once
/// \file
///
/// Computes the order of polygon vertices in 2D. (In 3D the "order" of polygon
/// vertices does not really make sense).
#include <hm3/geometry/algorithm/planar_projection.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/concept/polygon.hpp>

namespace hm3::geometry {

enum class vertex_order_t : unsigned char { ccw, cw };

namespace vertex_order_detail {

/// Returns the order of the xy components of the vertices of the polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr vertex_order_t order_2d(P&& p) {
  num_t tmp = 0.;
  for (auto&& e : edges(p)) {
    tmp += (e.x(1)(0) - e.x(0)(0)) * (e.x(1)(1) + e.x(0)(1));
  }
  return tmp < 0 ? vertex_order_t::ccw : vertex_order_t::cw;
}

struct vertex_order_fn {
  /// Returns the order of the vertices of the 2D polygon \p p.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
  constexpr vertex_order_t operator()(P&& p) const noexcept {
    return order_2d(std::forward<P>(p));
  }

  /// Returns the order of the vertices of the 3D _planar_ polygon \p p
  /// within its 2D plane.
  template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
  constexpr vertex_order_t operator()(P&& p) const noexcept {
    return order_2d(planar_projection.to(std::forward<P>(p)));
  }
};

}  // namespace vertex_order_detail

namespace {
constexpr auto const& vertex_order
 = static_const<vertex_order_detail::vertex_order_fn>::value;
}  // namespace

}  // namespace hm3::geometry
