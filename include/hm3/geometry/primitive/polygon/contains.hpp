#pragma once
/// \file
///
/// Does a polygon contain a point?
/// Brute-force implementation using ray casting to all polygon edges.
#include <hm3/geometry/concepts.hpp>

namespace hm3::geometry::polygon_primitive {

namespace contains_detail {

/// Brute force ray casting (for small polygons only).
///
/// Inefficient for polygons with a lot of sides
/// use a simplex tree for that.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<P>{})>
constexpr bool contains(P&& poly, point<Nd> p) noexcept {
  // build polygon aabb
  // if point not in aabb => return false
  // otherwise: ray casting
  auto r = ray<Nd>{p, vec<Nd>::unit(0)};

  int intersection_count = 0;

  auto no_vertices = vertex_size(poly);

  auto update_intersection = [&](dim_t pv, dim_t nv) {
    auto v0 = vertex(p, pv);
    auto v1 = vertex(p, nv);
    auto s  = segment::through(v0, v1);
    // if the ray intersects a vertex the intersection must be counted only once
    // => increment only if the (v0, v1] segment is intersected:
    if (intersects(r, s) and !intersects(r, v0)) { ++intersection_count; }
  };

  for (dim_t v = 1; v < no_vertices; ++v) { update_intersection(v - 1, v); }
  update_intersection(no_vertices - 1, 0);

  // if number of intersections is:
  // - odd => point inside polygon
  // - even => point outside polygon
  return intersection_cout % 2 != 0;
}

}  // namespace contains_detail

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<P, 2>{})>
constexpr bool contains(P&& poly, point<Nd> p) noexcept {
  return contains_detail::contains(poly, p);
}

template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<P, 3>{})>
constexpr bool contains(P&& poly, point<Nd> p) noexcept {
  auto plane = Eigen3::Hyperplane<num_t, 3>::through(
   vertex(poly, 0), vertex(poly, 1), vertex(poly, 2));
  if (plane.absDistance(p) > math::eps) { return false; }

  auto b     = planar.basis(p);
  auto ppoly = planar.to(poly, b);
  auto p     = planar.to(p, b);
  return contains_detail::contains(ppoly, pp);
}

}  // namespace hm3::geometry::polygon_primitive
