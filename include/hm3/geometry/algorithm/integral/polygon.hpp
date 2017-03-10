#pragma once
/// \file
///
/// Integral of a Polygon.
#include <hm3/geometry/algorithm/cross.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/integral/polyline.hpp>
#include <hm3/geometry/algorithm/normal/polygon.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/fwd.hpp>

/// This macro forces computing the signed area using a generic algorithm for
/// polygons with N vertices (it disables faster specializations for common
/// shapes like tris and quads).
//#define HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA

/// This macro forces computing the signed area of polygons with N vertices
/// using the algorithm based on the Stokes theorem which is way slower (the
/// macro is only useful for testing faster algorithms).
//#define HM3_GEOMETRY_POLYGON_SIGNED_AREA_STOKES

namespace hm3::geometry {

namespace integral_polygon_detail {

/// Compute the signed area of the two dimensional polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<uncvref_t<P>, 2>{})>
constexpr num_t signed_area(P&& p) noexcept {
  num_t a        = 0.;
  const auto& ps = vertices(p);
  const dim_t e  = ranges::size(ps) - 1;
  a += ps[0](0) * (ps[1](1) - ps[e](1));
  for (dim_t i = 1; i != e; ++i) {
    a += ps[i](0) * (ps[i + 1](1) - ps[i - 1](1));
  }
  a += ps[e](0) * (ps[0](1) - ps[e - 1](1));
  return a / 2.;
}

/// Compute the signed area of a three dimensional polygon \p p.
///
/// From Stokes theorem:
///
/// SA = n / 2 * sum_{i = 0}^{n-1} vi.cross(vi+1)
///
/// which uses 6n+3 multiplications and 4n+2 additions.
///
/// \note This is 6x slower than the approach followed below.
template <typename P>
constexpr num_t signed_area_stokes(P&& p) noexcept {
  static_assert(Polygon<P, 3>{});
  auto tmp         = vec<3>::constant(0.);
  auto no_vertices = vertex_size(p);
  for (dim_t v = 0; v < no_vertices - 1; ++v) {
    tmp() += vertex(p, v)().cross(vertex(p, v + 1)());
  }
  tmp() += vertex(p, no_vertices - 1)().cross(vertex(p, 0)());
  return 0.5 * normal_polygon(p)().dot(tmp());
}

/// Compute the signed area of a three dimensional polygon \p p.
///
/// Note: this method projects the polygon onto an axis-aligned 2D plane. See:
/// http://geomalgorithms.com/a01-_area.html
///
/// The method should be ~6x faster than Stokes method. The implementation has
/// been adapted from that website, seee copyright notice below.
///
template <typename P>
constexpr num_t signed_area_projection(P&& p) noexcept {
  static_assert(Polygon<P, 3>{});
  // Adapted from: http://geomalgorithms.com/a01-_area.html
  // Copyright 2000 softSurfer, 2012 Dan Sunday
  // This code may be freely used and modified for any purpose
  // providing that this copyright notice is included with it.
  // iSurfer.org makes no warranty for this code, and cannot be held
  // liable for any real or imagined damage resulting from its use.
  // Users of this code must verify correctness for their application.
  auto n    = normal_polygon(p);
  auto absn = n().array().abs();
  auto vs   = vertices(p);
  auto e    = vertex_size(p) - 1;
  HM3_ASSERT(math::approx(n().norm(), 1.0), "");
  // ignore largest normal component:
  dim_t x = 2;  // ignore z-coord
  if (absn(0) > absn(1)) {
    if (absn(0) > absn(2)) { x = 0; }  // ignore x-coord
  } else if (absn(1) > absn(2)) {
    x = 1;  // ignore y-coord
  }
  constexpr dim_t s0[3] = {1, 2, 0};
  constexpr dim_t s1[3] = {2, 0, 1};
  const dim_t x0        = ranges::at(s0, x);
  const dim_t x1        = ranges::at(s1, x);

  // compute area of the 2D projection
  num_t area = 0;
  for (dim_t i = 1; i < e; ++i) {
    area += (vs[i](x0) * (vs[i + 1](x1) - vs[i - 1](x1)));
  }
  area += (vs[0](x0) * (vs[1](x1) - vs[e](x1)));
  area += (vs[e](x0) * (vs[0](x1) - vs[e - 1](x1)));

  // scale to get area before projection
  area *= 0.5 / n(x);
  return area;
}

/// Computes the signed area of the three dimensional polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<uncvref_t<P>, 3>{})>
constexpr num_t signed_area(P&& p) noexcept {
#if defined(HM3_GEOMETRY_POLYGON_SIGNED_AREA_STOKES)
  return signed_area_stokes(std::forward<P>(p));
#else
  return signed_area_projection(std::forward<P>(p));
#endif  // Stokes
}

/// Area of a three-dimensional triangle
///
/// Uses Heron's formula: A = \sqrt{s(s - a)(s - b)(s - c)}
/// where s = p / 2 and p the perimeter of the triangle p = a + b + c.
template <typename Triangle>
constexpr num_t area_triangle(Triangle&& t) noexcept {
  HM3_ASSERT(vertex_size(t) == 3, "");
  auto v0 = vertex(t, 0);
  auto v1 = vertex(t, 1);
  auto v2 = vertex(t, 2);
  num_t a = (v0() - v1()).norm();
  num_t b = (v1() - v2()).norm();
  num_t c = (v2() - v0()).norm();

  num_t p = a + b + c;
  num_t s = .5 * p;
  return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

/// Area of the polygon \p p.
template <typename P>
constexpr num_t area(P&& p) noexcept {
#if !defined(HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA)
  if (vertex_size(p) == 3) { return area_triangle(std::forward<P>(p)); }
#endif
  return std::abs(signed_area(std::forward<P>(p)));
}

struct integral_polygon_fn {
  /// Surface integral of the polygon \p s.
  template <typename T, CONCEPT_REQUIRES_(Polygon<uncvref_t<T>, 2>{})>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::volume_integral<T>) const noexcept {
    return area(std::forward<T>(s));
  }

  /// Surface integral of the polygon \p s.
  template <typename T, CONCEPT_REQUIRES_(Polygon<uncvref_t<T>, 3>{})>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::surface_integral<T>) const noexcept {
    return area(std::forward<T>(s));
  }

  /// Boundary integral of the polygon \p s.
  template <typename T, typename UT = uncvref_t<T>,
            CONCEPT_REQUIRES_(Polygon<UT>{})>
  constexpr associated::num_type_t<T> operator()(
   T&& s, trait::boundary_integral<T>) const noexcept {
    using p_t = typename UT::polyline_t;
    return integral_polyline(s.polyline(), trait::path_integral<p_t>{});
  }
};

}  // namespace integral_polygon_detail

namespace {
constexpr auto const& integral_polygon
 = static_const<integral_polygon_detail::integral_polygon_fn>::value;
}  // namespace

}  // namespace hm3::geometry
