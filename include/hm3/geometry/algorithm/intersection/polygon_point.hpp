#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/ray_segment.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/geometry/primitive/ray.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::polygon_primitive {

/// Does the polygon \p poly intersect/contain the point \p p?
///
/// Tests whether a point is in a polygon using the Crossing Number algorithm,
/// see: http://geomalgorithms.com/a03-_inclusion.html
///
/// TODO: Implement the Winding Number method (which is ~always better than CN).
///
/// Algorithm (simplified):
///
/// create a ray in any direction
/// if the number of intersections between the ray and the polygon is odd:
///   => point inside the polygon
/// otherwise:
///   => point outside the polygon
///
/// This is implemented by computing the intersection of the ray with the
/// polygon segments.
template <typename P, dim_t Nd = dimension_v<P>,
          CONCEPT_REQUIRES_(Nd == 2 and Polygon<P, Nd>{})>
constexpr bool intersection_test(P const& poly, point<Nd> const& p) noexcept {
  const auto r = ray<Nd>(p, vec<Nd>::unit(0));

  sint_t no_crossings = 0;

  for (auto&& s : faces(poly)) {
    // if the point is on a face its inside the polygon:
    auto is = intersection(s, p);
    if (is.index() != 0) { return true; }
    auto ir = intersection(r, s);
    visit(
     [&](auto&& i) {
       using T = uncvref_t<decltype(i)>;
       if
         constexpr(Same<T, point<Nd>>{}) { ++no_crossings; }
     },
     ir);
  }

  return no_crossings % 2 != 0;  // even => outside, odd => inside
}

template <typename P, dim_t Nd = dimension_v<P>,
          CONCEPT_REQUIRES_(Nd == 3 and Polygon<P, Nd>{})>
constexpr bool intersection_test(P const& /*poly*/,
                                 point<Nd> const& /*p*/) noexcept {
  // auto plane = Eigen::Hyperplane<num_t, 3>::Through(
  //  vertex(poly, 0), vertex(poly, 1), vertex(poly, 2));
  // if (plane.absDistance(p) > math::eps) { return false; }

  // auto b     = planar.basis(p);
  // auto ppoly = planar.to(poly, b);
  // auto p     = planar.to(p, b);
  // return contains_detail::contains(ppoly, pp);
  HM3_FATAL_ERROR("unimplemented");
  return false;
}

/// Intersection of a polygon \p poly with a point \p p (either the point or
/// nothing).
template <typename P, dim_t Nd = dimension_v<P>,
          CONCEPT_REQUIRES_(Nd == 2 and Polygon<P, Nd>{})>
constexpr variant<monostate, point<Nd>> intersection(
 P&& poly, point<Nd> const& p) noexcept {
  if (intersection_test(poly, p)) { return p; }
  return monostate{};
}

}  // namespace hm3::geometry::segment_primitive
