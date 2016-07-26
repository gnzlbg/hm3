#pragma once
/// \file
///
/// Area of a polygon
#include <hm3/geometry/discrete/algorithm/distance.hpp>
#include <hm3/geometry/discrete/algorithm/integral.hpp>
#include <hm3/geometry/discrete/primitive/polygon/concept.hpp>

/// This macro forces computing the signed area using a generic algorithm
/// for polygons with N vertices (it disables faster specializations for
/// common shapes like tris and quads).
#define HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA

/// This macro forces computing the signed area of polygons with N vertices
/// using the algorithm based on the Stokes theorem which is way slower (the
/// macro is only useful for testing faster algorithms).
#define HM3_GEOMETRY_POLYGON_SIGNED_AREA_STOKES

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

namespace integral_detail {

/// Compute the signed area of the two dimensional polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr num_t signed_area(P&& p) noexcept {
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
  auto tmp         = vector<3>::constant(0.);
  auto no_vertices = vertex_size(p);
  for (dim_t v = 0; v < no_vertices - 1; ++v) {
    tmp() += vertex(p, v)().cross(vertex(p, v + 1)());
  }
  return 0.5 * n().dot(tmp());
}

/// Compute the signed area of a three dimensional polygon \p p.
///
/// Note: this basically uses brute force:
///
/// SA = n / 2 * sum_{i = 0}^{n-1} vi.cross(vi+1)
///
/// which uses 6n+3 multiplications and 4n+2 additions.
///
/// There is a more efficient approach described here:
///
/// n+5 multiplications, 2n+1 additions, 1
///
/*template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr num_t signed_area(P&& p) noexcept {
  float area = 0;
  float an, ax, ay, az;  // abs value of normal and its coords
  int coord;             // coord to ignore: 1=x, 2=y, 3=z
  int i, j, k;           // loop indices

  // select largest abs coordinate to ignore for projection
  float ax = (N.x > 0 ? N.x : -N.x);  // abs x-coord
  float ay = (N.y > 0 ? N.y : -N.y);  // abs y-coord
  float az = (N.z > 0 ? N.z : -N.z);  // abs z-coord

  int coord = 3;  // ignore z-coord
  if (ax > ay) {
    if (ax > az) coord = 1;  // ignore x-coord
  } else if (ay > az)
    coord = 2;  // ignore y-coord

  // compute area of the 2D projection
  switch (coord) {
    case 1:
      for (i = 1, j = 2, k = 0; i < n; i++, j++, k++)
        area += (V[i].y * (V[j].z - V[k].z));
      break;
    case 2:
      for (i = 1, j = 2, k = 0; i < n; i++, j++, k++)
        area += (V[i].z * (V[j].x - V[k].x));
      break;
    case 3:
      for (i = 1, j = 2, k = 0; i < n; i++, j++, k++)
        area += (V[i].x * (V[j].y - V[k].y));
      break;
  }
  switch (coord) {  // wrap-around term
    case 1: area += (V[n].y * (V[1].z - V[n - 1].z)); break;
    case 2: area += (V[n].z * (V[1].x - V[n - 1].x)); break;
    case 3: area += (V[n].x * (V[1].y - V[n - 1].y)); break;
  }

  // scale to get area before projection
  an = sqrt(ax * ax + ay * ay + az * az);  // length of normal vector
  switch (coord) {
    case 1: area *= (an / (2 * N.x)); break;
    case 2: area *= (an / (2 * N.y)); break;
    case 3: area *= (an / (2 * N.z));
  }
  return area;
  }*/

/// Computes the signed area of the three dimensional polygon \p p.
template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr num_t signed_area(P&& p) noexcept {
#if defined(HM3_GEOMETRY_POLYGON_SIGNED_AREA_STOKES)
  return signed_area_stokes(std::forward<P>(p));
#else
  return signed_area_fast(std::forward<P>(p));
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
template <typename P>  //
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

}  // namepsace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
