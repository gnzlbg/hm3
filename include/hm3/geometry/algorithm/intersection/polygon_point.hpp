#pragma once
/// \file
///
/// Intersection of line segments with points.
#include <hm3/geometry/algorithm/intersection/ray_segment.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>

namespace hm3::geometry {

namespace intersection_test_polygon_point_detail {

/// Is the point \p p on any of the edges of the polygon \p pg ?
template <typename P, typename PG>
constexpr bool is_point_on_polygon_edge(P const& p, PG const& pg, num_t abs_tol,
                                        num_t rel_tol) noexcept {
  static_assert(Polygon<PG>{});
  static_assert(Point<P>{});
  static_assert(ad_v<PG> == ad_v<P>);
  static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
  static_assert(ad_v<PG> == 2,
                "3D polygon point intersection not implemented (TODO)");
  return ranges::any_of(edges(pg), [&](auto&& e) {
    return intersection_test_segment_point(e, p, abs_tol, rel_tol);
  });
}

/// Compute whether a point is inside a polygon using the crossing
/// number algorithm.
///
/// \returns true if the point is inside or on an edge, false if the point is
/// outside.
///
/// \complexity O(E) where E is the number of edges in the polygon. Consider
/// using an acceleration structure for large number of edges.
///
/// Crossing Number algorithm: http://geomalgorithms.com/a03-_inclusion.html
///
/// Create a ray in any direction
///  -  if the number of intersections between the ray and the polygon is odd:
///     => point inside the polygon
///  - otherwise:
///     => point outside the polygon
///
/// This is implemented by computing the intersection of the ray with all of the
/// polygon edges (O(E)).
template <typename P, typename PG>
constexpr bool is_inside_crossing_number(P const& p, PG const& pg,
                                         num_t abs_tol,
                                         num_t rel_tol) noexcept {
  static_assert(Polygon<PG>{});
  static_assert(Point<P>{});
  static_assert(ad_v<PG> == ad_v<P>);
  static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
  static_assert(ad_v<PG> == 2,
                "3D polygon point intersection not implemented (TODO)");

  using v_t = associated::vector_t<PG>;
  using r_t = associated::ray_t<PG>;

  // otherwise, count the number of times the ray intersects the polygon
  // edges:
  const auto r = r_t(p, v_t::unit(0));

  sint_t no_crossings = 0;
  for (auto&& s : edges(pg)) {
    auto ir = intersection_ray_segment(r, s, abs_tol, rel_tol);
    visit(
     [&](auto&& i) {
       using T = uncvref_t<decltype(i)>;
       if constexpr (Same<T, P>{}) { ++no_crossings; }
     },
     ir);
  }

  return no_crossings % 2 != 0;  // even => outside, odd => inside
}

/// Is a point inside a polygon (or on its edge)?
///
/// \complexity O(E) where E is the number of edges.
///
/// Currently uses the crossing number algorithm.
///
/// TODO: Implement the Winding Number method which is ~always better than CN.
template <typename P, typename PG>
constexpr bool is_inside(P const& p, PG const& pg, num_t abs_tol,
                         num_t rel_tol) noexcept {
  static_assert(Polygon<PG>{});
  static_assert(Point<P>{});
  static_assert(ad_v<PG> == ad_v<P>);
  static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
  static_assert(ad_v<PG> == 2,
                "3D polygon point intersection not implemented (TODO)");
  return is_inside_crossing_number(p, pg, abs_tol, rel_tol);
}

struct intersection_test_polygon_point_fn {
  /// Does the polygon \p poly intersect/contain the point \p p?
  ///
  /// \complexity O(E) where E is the number of edges.
  ///
  /// \warning For large polygons consider using an acceleration structure.
  template <typename PG, typename P>
  constexpr bool operator()(PG const& poly, P const& p, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Polygon<PG>{});
    static_assert(Point<P>{});
    static_assert(ad_v<PG> == ad_v<P>);
    static_assert(Same<associated::point_t<PG>, associated::point_t<P>>{});
    static_assert(ad_v<PG> == 2,
                  "3D polygon point intersection not implemented (TODO)");

    // if the point is on an edge of the polygon => it is inside it (we are
    // done)
    if (is_point_on_polygon_edge(p, poly, abs_tol, rel_tol)) { return true; }

    // otherwise, count the number of times the ray intersects the polygon
    // edges:
    return is_inside(p, poly, abs_tol, rel_tol);
  }
};

}  // namespace intersection_test_polygon_point_detail

namespace {
constexpr auto const& intersection_test_polygon_point
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
constexpr auto const& intersection_polygon_point
 = static_const<with_default_tolerance<
  intersection_polygon_point_detail::intersection_polygon_point_fn>>::value;
}  // namespace

}  // namespace hm3::geometry
