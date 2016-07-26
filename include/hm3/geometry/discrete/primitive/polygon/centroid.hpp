#pragma once
/// \file
///
/// Centroid of a ccw-sorted polygon.
#include <hm3/geometry/discrete/algorithm/centroid.hpp>
#include <hm3/geometry/discrete/primitive/polygon/concept.hpp>
#include <hm3/geometry/discrete/primitive/polygon/degenerate.hpp>
#include <hm3/geometry/discrete/primitive/polygon/integral.hpp>

/// This macro forces computing the centroid using a generic algorithm
/// for polygons with N vertices (it disables faster specializations for
/// common shapes like tris and quads).
#define HM3_GEOMETRY_POLYGON_USE_GENERIC_SIGNED_AREA

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

namespace centroid_detail {

template <typename P>
constexpr auto centroid_2d(P&& p) noexcept -> point<dimension_t<P>{}> {
  HM3_ASSERT(vertex_order.counter_clockwise(p),
             "polygon {} not sorted counter clock wise!", p);
  auto x       = point<dimension_t<P>{}>::constant(0.);
  auto a       = integral_detail::signed_area(p);
  auto&& ps    = vertices(p);
  const auto e = size(ps) - 1;

  auto kernel = [&](auto&& l, auto&& r) {
    return (ps[l](0) * ps[r](1) - ps[r](0) * ps[l](1)) * (ps[l]() + ps[r]());
  };

  for (dim_t i = 0; i < e; ++i) { x() += kernel(i, i + 1); }
  {  // wrap around
    x() += kernel(e, 0);
  }

  x() /= (6. * a);
  return x;
}

/// Centroid of a triangle
template <typename Triangle>
constexpr auto centroid_triangle(Triangle&& t) noexcept
 -> point<dimension_t<Triangle>{}> {
  HM3_ASSERT(vertex_size(t) == 3, "");
  constexpr num_t f = 1. / 3.;

  auto v0 = vertex(t, 0);
  auto v1 = vertex(t, 1);
  auto v2 = vertex(t, 2);
  return point<3>(f * (v0() + v1() + v2()));
}

}  // namespace centroid_detail

template <typename P, CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr auto centroid(P&& p) noexcept -> point<dimension_v<P>> {
  return centroid_detail::centroid_2d(std::forward<P>(p));
}

template <typename P, CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr auto centroid(P&& p) noexcept -> point<dimension_v<P>> {
#if !defined(HM3_GEOMETRY_POLYGON_USE_GENERIC_CENTROID)
  if (vertex_size(p) == 3) {
    return centroid_detail::centroid_triangle(std::forward<P>(p));
  }
#endif
  // transforms polygon into planar coordinates:
  auto b  = planar.basis(p);
  auto pp = planar.to(p, b);
  // compute centroid in planar coordinates:
  auto pc = centroid_detail::centroid_2d(pp);
  // transform centroid to 3D coordinates:
  return planar.from(pc, b);
}

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
