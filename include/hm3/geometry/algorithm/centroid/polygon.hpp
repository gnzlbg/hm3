#pragma once
/// \file
///
/// Centroid of a polygon
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/integral/polygon.hpp>
#include <hm3/geometry/algorithm/planar_projection.hpp>
#include <hm3/geometry/algorithm/vertex_order.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/fwd.hpp>

/// This macro forces computing the centroid using a generic algorithm
/// for polygons with N vertices (it disables faster specializations for
/// common shapes like tris and quads).
/// #define HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA

namespace hm3::geometry {

namespace centroid_polygon_detail {

/// Centroid of the first 2 (x,y) coordinates of a polygon
template <typename P>
static constexpr auto centroid_2d(P&& p) noexcept {
  static_assert(Polygon<uncvref_t<P>>{});
  using p_t = associated::point_t<P>;

  auto x = p_t::constant(0.);
  for (auto&& e : edges(p)) {
    x()
     += (e.x(0)(0) * e.x(1)(1) - e.x(1)(0) * e.x(0)(1)) * (e.x(0)() + e.x(1)());
  }

  x() /= (6. * integral_polygon_detail::signed_area(p));

  if (vertex_order(p) != vertex_order_t::ccw) { x() *= -1.; }
  return x;
}

struct centroid_polygon_fn {
  /// Centroid of 2D polygon \p p.
  template <typename P>
  static constexpr auto impl(P&& p, trait::polygon<2>) noexcept {
    static_assert(Polygon<uncvref_t<P>, 2>{});
    return centroid_2d(p);
  }

  /// Centroid of 3D polygon \p p.
  template <typename P>
  static constexpr auto impl(P&& p, trait::polygon<3>) noexcept {
    static_assert(Polygon<uncvref_t<P>, 3>{});
    // transforms polygon into planar coordinates:
    auto b  = planar_basis(p);
    auto pp = planar_projection.to(p, b);
    // compute centroid in planar coordinates:
    auto pc = centroid_2d(pp);
    // transform centroid to 3D coordinates:
    return planar_projection.from(pc, b);
  }

  /// Centroid of a triangle \p t.
  template <typename P, dim_t Ad = ad_v<P>>
  static constexpr auto impl(P&& t, trait::triangle<Ad>) noexcept {
    static_assert(Polygon<uncvref_t<P>>{}
                  and Same<associated::t_<P>, trait::triangle<Ad>>{});
    HM3_ASSERT(vertex_size(t) == 3, "");
    using p_t         = associated::point_t<P>;
    constexpr num_t f = 1. / 3.;

    auto v0 = vertex(t, 0);
    auto v1 = vertex(t, 1);
    auto v2 = vertex(t, 2);
    return p_t(f * (v0() + v1() + v2()));
  }

  /// Centroid coordinates of the polygon \p s.
  template <typename P>
  constexpr auto operator()(P&& p) const noexcept {
    static_assert(Polygon<uncvref_t<P>>{});
    return impl(std::forward<P>(p), associated::v_<P>);
  }
};

}  // namespace centroid_polygon_detail

namespace {
constexpr auto const& centroid_polygon
 = static_const<centroid_polygon_detail::centroid_polygon_fn>::value;
}

}  // namespace hm3::geometry
