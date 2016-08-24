#pragma once
/// \file
///
/// Area of a polygon
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/integral.hpp>
#include <hm3/geometry/concepts.hpp>

/// This macro forces computing the signed area using a generic algorithm for
/// polygons with N vertices (it disables faster specializations for common
/// shapes like tris and quads).
//#define HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA

/// This macro forces computing the signed area of polygons with N vertices
/// using the algorithm based on the Stokes theorem which is way slower (the
/// macro is only useful for testing faster algorithms).
//#define HM3_GEOMETRY_POLYGON_SIGNED_AREA_STOKES

namespace hm3::geometry::polygon_primitive {

namespace integral_detail {

/// Compute the signed area of the two dimensional polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr num_t signed_area(P&& p) noexcept {
  // not necessary:
  // HM3_ASSERT(vertex_order.counter_clockwise(p),
  //            "polygon vertices not in ccw order: {}", p);
  num_t a        = 0.;
  const auto& ps = vertices(p);
  const dim_t e  = size(ps) - 1;
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
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr num_t signed_area_stokes(P&& p) noexcept {
  auto n           = normal(p);
  auto tmp         = vec<3>::constant(0.);
  auto no_vertices = vertex_size(p);
  for (dim_t v = 0; v < no_vertices - 1; ++v) {
    tmp() += vertex(p, v)().cross(vertex(p, v + 1)());
  }
  tmp() += vertex(p, no_vertices - 1)().cross(vertex(p, 0)());
  return 0.5 * n().dot(tmp());
}

/// Compute the signed area of a three dimensional polygon \p p.
///
/// Note: this method projects the polygon onto an axis-aligned 2D plane. See:
/// http://geomalgorithms.com/a01-_area.html
///
/// The method should be ~6x faster than Stokes method. The implementation has
/// been adapted from that website, seee copyright notice below.
///
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr num_t signed_area_projection(P&& p) noexcept {
  // Adapted from: http://geomalgorithms.com/a01-_area.html
  // Copyright 2000 softSurfer, 2012 Dan Sunday
  // This code may be freely used and modified for any purpose
  // providing that this copyright notice is included with it.
  // iSurfer.org makes no warranty for this code, and cannot be held
  // liable for any real or imagined damage resulting from its use.
  // Users of this code must verify correctness for their application.
  auto n    = normal(p);
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
  const dim_t x0        = s0[x];
  const dim_t x1        = s1[x];

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
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
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

}  // namespace integral_detail

/// Computes the absolute area of the polygon \p p.
///
/// 3D version: surface integral.
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr num_t integral(P&& p, surface_integral<P>) noexcept {
  return integral_detail::area(std::forward<P>(p));
}

/// Computes the absolute area of the polygon \p p.
///
/// 2D version: volume integral.
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr num_t integral(P&& p, volume_integral<P>) noexcept {
  return integral_detail::area(std::forward<P>(p));
}

/// Compute the boundary of the polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr num_t integral(P&& p, boundary_integral<P>) noexcept {
  num_t l = 0.;
  // for (auto&& f : faces(p)) {
  //    l += integral(f, path);
  // }
  // return l;

  auto no_vertices = vertex_size(p);
  for (dim_t v = 0; v < no_vertices - 1; ++v) {
    l += std::abs(distance.centroid(vertex(p, v), vertex(p, v + 1)));
  }
  l += std::abs(distance.centroid(vertex(p, no_vertices - 1), vertex(p, 0)));
  return l;
}

}  // namepsace hm3::geometry::polygon_primitive
