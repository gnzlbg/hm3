#pragma once
/// \file
///
/// Intersection of a polygon with a polyline.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/polyline_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/split/polyline_point.hpp>
#include <hm3/geometry/algorithm/split/range.hpp>

namespace hm3::geometry {

namespace intersection_polygon_polyline_detail {

struct intersection_polygon_polyline_fn {
  /// Computes the intersection between the polygon \p pg and the polyline \p
  /// pl.
  ///
  /// The result consists of:
  ///
  /// - a set of polylines completely inside the polygon, each having two
  ///   intersection points with the polygon.
  /// - a set of points on the polygon edges that are not part of any of the
  ///   polylines intersecting the polygon.
  ///
  /// If the polyline is completely inside the polygon, this returns the
  /// polyline itself.
  template <typename PG, typename PL, typename UPG = uncvref_t<PG>>
  constexpr auto operator()(PG&& pg, PL&& pl, num_t abs_tol,
                            num_t rel_tol) const noexcept {
    static_assert(Polygon<UPG>{});
    static_assert(Polyline<uncvref_t<PL>>{});
    static_assert(ad_v<PG> == ad_v<PL>);

    using p_t = associated::point_t<PL>;

    // 1. Compute intersection points between polygon and polyline:
    //
    // Complexity:  O(N_pg x N_pl x N_ip)

    small_vector<p_t, 10> intersection_points;

    auto equal_point
     = [&](auto&& x, auto&& y) { return approx_point(x, y, abs_tol, rel_tol); };
    auto push_back_ip
     = [&](auto&& p) { unique_push_back(intersection_points, p, equal_point); };

    for (auto&& pg_s : edges(pg)) {    // O(N_pg)
      for (auto&& pl_s : edges(pl)) {  // O(N_pl)
        auto i = intersection_segment_segment(pg_s, pl_s, abs_tol, rel_tol);
        visit(
         [&](auto&& v) {
           using T = uncvref_t<decltype(v)>;
           if constexpr (Point<T>{}) {
             push_back_ip(v);
           }  // O(N_ip)
           else if constexpr (Segment<T>{}) {
             push_back_ip(v.x(0));  // O(N_ip)
             push_back_ip(v.x(1));  // O(N_ip)
           } else if constexpr (Same<T, monostate>{}) {
           } else {
             HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
           }
         },
         i);
      }
    }

    // 2. Split polyline at the intersection points => range of polylines
    //
    // If the polyline is completely inside or outside the polygon, this returns
    // the polyline itself.
    auto intersection_polylines
     = split_range(pl, intersection_points, split_polyline_point);

    // 3. Remove polylines outside the polygon => range of polylines inside
    // polygon.
    //
    // A polyline is outside the polygon if any of its vertices is outside the
    // polygon.
    {
      auto is_outside = [&](auto&& l) {
        return ranges::any_of(vertices(l), [&](auto&& vx) {
          return not intersection_test_polygon_point(pg, vx, abs_tol, rel_tol);
        });
      };

      ranges::action::remove_if(intersection_polylines, is_outside);
    }

    // 4. The intersection points that are part of a polyline are not part of
    // the intersection. The points that remain are part of the intersection,
    // but not part of any polyline.
    {
      auto in_polyline = [&](auto&& ip) {
        return ranges::any_of(intersection_polylines, [&](auto&& l) {
          return intersection_test_polyline_point(l, ip, abs_tol, rel_tol);
        });
      };

      ranges::action::remove_if(intersection_points, in_polyline);
    }

    // The result is the union of the intersection points (that do not belong to
    // any intersected polyline) and the intersection polylines:
    return std::make_pair(intersection_points, intersection_polylines);
  }
};

}  // namespace intersection_polygon_polyline_detail

namespace {
constexpr auto const& intersection_polygon_polyline
 = static_const<with_default_tolerance<
  intersection_polygon_polyline_detail::intersection_polygon_polyline_fn>>::
  value;
}

}  // namespace hm3::geometry
