#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/intersection/ray_segment.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>

namespace hm3::geometry {

namespace intersection_test_polygon_point_detail {

struct intersection_test_polygon_point_fn {
  /// Does the polygon \p poly intersect/contain the point \p p?
  ///
  /// Tests whether a point is in a polygon using the Crossing Number algorithm,
  /// see: http://geomalgorithms.com/a03-_inclusion.html
  ///
  /// TODO: Implement the Winding Number method (which is ~always better than
  /// CN).
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
  template <typename PG, typename P>
  constexpr bool operator()(PG const& poly, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Polygon<PG>{});
    static_assert(Point<P>{});
    static_assert(ad_v<PG> == ad_v<P>);
    static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
    static_assert(ad_v<PG> == 2,
                  "3D polygon point intersection not implemented (TODO)");

    using v_t = associated::vector_t<PG>;
    using r_t = associated::ray_t<PG>;

    // if the point is on an edge of the polygon => it is inside it (we are
    // done)
    for (auto&& s : edges(poly)) {
      if (intersection_test_segment_point(s, p, abs_tol, rel_tol)) {
        return true;
      }
    }

    // otherwise, count the number of times the ray intersects the polygon
    // edges:
    const auto r = r_t(p, v_t::unit(0));

    sint_t no_crossings = 0;
    for (auto&& s : edges(poly)) {
      auto ir = intersection_ray_segment(r, s, abs_tol, rel_tol);
      visit(
       [&](auto&& i) {
         using T = uncvref_t<decltype(i)>;
         if
           constexpr(Same<T, P>{}) { ++no_crossings; }
       },
       ir);
    }

    return no_crossings % 2 != 0;  // even => outside, odd => inside
  }
};

}  // namespace intersection_test_polygon_point_detail

namespace {
static constexpr auto const& intersection_test_polygon_point
 = static_const<with_default_tolerance<
  intersection_test_polygon_point_detail::intersection_test_polygon_point_fn>>::
  value;
}  // namespace

namespace intersection_polygon_point_detail {

struct intersection_polygon_point_fn {
  /// Intersection of a polygon \p poly with a point \p p (either the point or
  /// nothing).
  template <typename PG, typename P>
  constexpr variant<monostate, P> operator()(PG&& poly, P const& p,
                                             num_t abs_tol, num_t rel_tol) const
   noexcept {
    static_assert(Polygon<PG>{});
    static_assert(Point<P>{});
    static_assert(ad_v<PG> == ad_v<P>);
    static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
    static_assert(ad_v<PG> == 2,
                  "3D polygon point intersection not implemented (TODO)");

    if (intersection_test_polygon_point(poly, p, abs_tol, rel_tol)) {
      return p;
    }
    return monostate{};
  }
};

}  // namespace intersection_polygon_point_detail

namespace {
static constexpr auto const& intersection_polygon_point
 = static_const<with_default_tolerance<
  intersection_polygon_point_detail::intersection_polygon_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
