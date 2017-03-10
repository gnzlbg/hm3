#pragma once
/// \file
///
/// Intersection of a polygon with a signed-distance field.
#include <hm3/geometry/algorithm/approx.hpp>
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/algorithm/direction.hpp>
#include <hm3/geometry/algorithm/intersection/polyline_point.hpp>
#include <hm3/geometry/algorithm/is_convex.hpp>
#include <hm3/geometry/algorithm/sd_intersection/triangle.hpp>
#include <hm3/geometry/algorithm/sd_intersection_test.hpp>
#include <hm3/geometry/algorithm/simplify.hpp>

namespace hm3::geometry::sd {

namespace intersection_polygon_detail {

template <typename P>
struct intersection_result {
  using point_t    = associated::point_t<P>;
  using polyline_t = associated::polyline_t<P>;

  small_vector<point_t, 4> points;
  small_vector<polyline_t, 4> polylines;

  /// True if there is an intersection.
  explicit operator bool() const noexcept {
    return not points.empty() or not polylines.empty();
  }

  bool operator==(intersection_result const& o) const {
    return points == o.points and polylines == o.polylines;
  }
  bool operator!=(intersection_result const& o) const {
    return !((*this) == o);
  }
};

struct intersection_polygon_fn {
  template <typename P, typename SDF>
  constexpr auto operator()(P&& poly, SDF&& sdf, num_t abs_tol,
                            num_t rel_tol) const noexcept
   -> intersection_result<uncvref_t<P>> {
    using p_t  = associated::point_t<P>;
    using e_t  = associated::edge_t<P>;
    using pl_t = associated::polyline_t<P>;
    using pg_t = uncvref_t<P>;
    using r_t  = intersection_result<pg_t>;
    static_assert(Polygon<pg_t>{});
    static_assert(SignedDistance<uncvref_t<SDF>, p_t>{});
    static_assert(ad_v<pg_t> == ad_v<SDF>);

    HM3_ASSERT(is_convex(poly), "TODO: add support for non-convex polygons");
    // TODO: split non-convex polygons into a sequence of convex polygons,
    // intersect those using this function, and merge the result afterwards.

    r_t result;

    // Utility functions
    //@{

    // Are two primitives equal?
    auto equal
     = [&](auto&& a, auto&& b) { return approx(a, b, abs_tol, rel_tol); };

    // Are two polylines equal?
    auto equal_pl = [&](auto&& a, auto&& b) {
      return equal(a, b) or equal(a, direction.invert(b));
    };

    // Appends polyline to the result if it isn't there yet.
    auto unique_push_polyline
     = [&](auto&& s) { unique_push_back(result.polylines, s, equal_pl); };

    // Appends point to the result if it isn't there yet.
    auto unique_push_point
     = [&](auto&& p) { unique_push_back(result.points, p, equal); };

    // Append a triangle-sdf intersection to the polygon-sdf intersection
    // result:q
    auto append_tri_intersection = [&](auto&& v) {
      using T = uncvref_t<decltype(v)>;
      if
        constexpr(Same<T, monostate>{}) { return; }
      else if
        constexpr(Same<T, p_t>{}) { unique_push_point(v); }
      else if
        constexpr(Same<T, e_t>{}) { unique_push_polyline(pl_t(edges(v))); }
      else if
        constexpr(Same<T, pl_t>{}) { unique_push_polyline(v); }
      else if
        constexpr(Same<T, pair<p_t, e_t>>{}) {
          unique_push_point(first(v));
          unique_push_polyline(pl_t(edges(second(v))));
        }
      else if
        constexpr(Same<T, pg_t>{}) { unique_push_polyline(pl_t(edges(v))); }
      else {
        HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
      }
    };

    // Makes a triangle from an edge and a point.
    auto make_triangle = [](e_t const& s, p_t const& o) {
      auto vxs = {s.x(0), s.x(1), o};
      return pg_t(make_segment_range(vxs));
    };

    //@} // Utility functions

    // If it's a triangle, compute the intersection and we are done:
    if (vertex.size(poly) == 3) {
      auto ir = intersection_triangle(poly, sdf);
      visit(append_tri_intersection, ir);
      return result;
    }

    // If polygon is not intersected we are done:
    if (not intersection_test(poly, sdf)) { return result; }

    // Compute the intersection of the polygon with the signed-distance function
    // by:
    //  - "splitting" the convex polygon into a set of triangles formed with a
    //    polygon edge and the polygon's centroid,
    //  - appending the intersection of each triangle with the sdf to the
    //    result.
    auto xc = centroid(poly);
    for (auto&& s : edges(poly)) {
      auto tri = make_triangle(s, xc);
      auto ir  = intersection_triangle(tri, sdf);
      visit(append_tri_intersection, ir);
    }
    // Clean up the result a bit:

    // Merge and simplify all the polylines:
    result.polylines = concatenate.range(result.polylines);
    for (auto&& pl : result.polylines) { pl = simplify(pl); }

    // Remove the points that are part of a polyline:
    action::remove_if(result.points, [&](auto&& p) {
      return ranges::any_of(result.polylines, [&](auto&& pl) {
        return intersection_test_polyline_point(pl, p);
      });
    });

    HM3_ASSERT(result, "result is empty (aka no intersection)?");
    return result;
  }
};

}  // namespace intersection_polygon_detail

namespace {
constexpr auto const& intersection_polygon
 = static_const<with_default_tolerance<
  intersection_polygon_detail::intersection_polygon_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd

namespace hm3::ascii_fmt {

template <typename OStream, typename P>
OStream& to_ascii(
 OStream& os,
 ::hm3::geometry::sd::intersection_polygon_detail::intersection_result<P> const&
  r,
 geometry const&) {
  os << "[polygon-sdf intersection result | points: ";
  to_ascii(os, r.points);
  os << ", polylines: ";
  to_ascii(os, r.polylines);
  os << "]";
  return os;
}

}  // namespace hm3::ascii_fmt
