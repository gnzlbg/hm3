#pragma once
/// \file
///
/// Split polygon with a polyline.
///
/// This is performed by the function `split_polygon_polyline`, which:
///
/// 1. Splits the polyline with the intersection points between the polyline and
/// the polygon, resulting in multiple polylines, with at most two intersection
/// points with the polygon.
///
/// 2. Clips these polylines to the polygon, by removing the polylines that are
/// outside the polygon.
///
/// 3. Recursively splits the polygon against the remaining polylines using
/// `binary_split`, since the end points of these polylines are guaranteed to
/// to intersect the polygon boundary, and the inner segments of the polyline do
/// not.

#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_polyline.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/split/polyline_point.hpp>
#include <hm3/geometry/algorithm/split/range.hpp>

namespace hm3::geometry {

namespace split_polygon_polyline_detail {

struct binary_split_fn {
  /// Splits the polygon \p pg with the polyline \p pl into 1 or two polygons.
  ///
  /// \pre There are exactly two intersection points between the polygon and the
  /// polyline.
  ///
  /// \pre The polyline enpoints intersect the polygon boundary.
  ///
  /// \pre No other polyline edges intersect the boundary.
  ///
  /// Works as follows:
  /// - 1. Assert that the preconditions are met.
  /// - 2. Find the polygon edges intersected by the polyline:
  ///   - They can be different edges, or the same edge.
  template <typename PG, typename PL, typename UPG = uncvref_t<PG>>
  constexpr fixed_capacity_vector<UPG, 2> operator()(PG&& pg, PL&& pl) const
   noexcept {
    static_assert(Polygon<UPG>{});
    static_assert(Polyline<uncvref_t<PL>>{});
    static_assert(ad_v<PG> == ad_v<PL>);

    using p_t = associated::point_t<PG>;
    using e_t = associated::edge_t<PG>;

    static_assert(
     Constructible<e_t, associated::edge_t<PL>>{},
     "The polygon edges must be constructible from the polyline edges");

    // Check the preconditions:
    auto assert_intersection_point = [&](auto&& p) {
      auto r = intersection_polygon_point(pg, p);
      return visit(
       [&](auto&& v) {
         if constexpr (Point<uncvref_t<decltype(v)>>{}) {
           return approx_point(v, p);
         }
         return false;
       },
       r);
    };

    HM3_ASSERT(assert_intersection_point(vertex.first(pl)),
               "The first polyline vertex must intersect the polygon "
               "boundary:\n\npolygon:{}\n\npolyline:{}\n",
               pg, pl);
    HM3_ASSERT(assert_intersection_point(vertex.last(pl)),
               "The last polyline vertex must intersect the polygon "
               "boundary:\n\npolygon:{}\n\npolyline:{}\n",
               pg, pl);

    auto count_intersections = [&]() {  // count intersection points of pg/pl
      small_vector<p_t, 12> ips;
      for (auto&& pg_e : edges(pg)) {
        for (auto&& pl_e : edges(pl)) {
          auto i = intersection_segment_segment(pg_e, pl_e);
          visit(
           [&](auto&& v) {
             using T = uncvref_t<decltype(v)>;
             if constexpr (Segment<T>{}) {
               unique_push_back(ips, v.x(0), approx_point);
               unique_push_back(ips, v.x(1), approx_point);
             } else if constexpr (Point<T>{}) {
               unique_push_back(ips, v, approx_point);
             }
           },
           i);
        }
      }
      return ranges::size(ips);
    };

    HM3_ASSERT(count_intersections() == 2,
               "The number of intersection points between the polyline "
               "and the polygon must be exactly "
               "two but is {}:\n\npolygon:{}\n\npolyline:{}\n",
               count_intersections(), pg, pl);

    // Create a polyline from the polygon edges and split it against the
    // intersection points:
    using pg_pl_t = associated::polyline_t<PG>;

    array<p_t, 2> intersection_points{{vertex.first(pl), vertex.last(pl)}};
    pg_pl_t pg_pl(edges(pg));
    auto pg_pls = split_range(pg_pl, intersection_points, split_polyline_point);

    // If the polygon's origin is not an intersection point, merge the polylines
    // at both sides of the origin:
    if (not approx_point(intersection_points[0], vertex.first(pg))
        and not approx_point(intersection_points[1], vertex.first(pg))) {
      fixed_capacity_vector<pg_pl_t, 2> pls_at_origin;
      auto o      = vertex.first(pg);
      suint_t idx = 0;
      while (idx < pg_pls.size()) {
        auto&& pl_ = pg_pls[idx];
        if (approx_point(vertex.first(pl_), o)
            or approx_point(vertex.last(pl_), o)) {
          pls_at_origin.push_back(pl_);
          pg_pls.erase(ranges::begin(pg_pls) + idx);
        } else {
          ++idx;
        }
      }
      HM3_ASSERT(pls_at_origin.size() == 2, "");
      auto rpl
       = concatenate.direction_independent(pls_at_origin[0], pls_at_origin[1]);
      HM3_ASSERT(rpl, "");
      pg_pls.push_back(rpl.value());
    }

    HM3_ASSERT(pg_pls.size() == 2,
               "The polygon's polyline must be split into "
               "two polylines:\n\npolygon: {}\n\npolyline: "
               "{}\n\nsplit with intersection points: "
               "{}\n",
               pg, pl, pg_pls);

    // Create an inversion of the original polyline:
    auto pl_i = direction.invert(pl);

    // Returns the polyline edges such that they can be concatenated with the
    // edges of the polygon \p pg_pl.
    //
    // If the last vertex of the polygon's polyline does not match the first of
    // the input polyline, we need use the reversed input polyline.
    auto polyline_edges = [&](auto&& pg_pl) {
      auto to_pg_es = [](auto&& e) { return e_t{e}; };
      if (approx_point(vertex.last(pg_pl), vertex.first(pl))) {
        return edges(pl) | view::transform(to_pg_es);
      } else {
        return edges(pl_i) | view::transform(to_pg_es);
      }
    };

    fixed_capacity_vector<UPG, 2> result;

    // If the polyline lies on a polygon edge, the result is only one polygon
    // (and the second polygon will have less than 3 edges):
    auto append_polygon = [&](auto&& pg_pl_m) {
      auto rng = view::concat(edges(pg_pl_m), polyline_edges(pg_pl_m));
      if (ranges::size(rng) < 3) { return; }
      result.push_back(UPG(rng));
    };

    append_polygon(pg_pls[0]);
    append_polygon(pg_pls[1]);

    HM3_ASSERT(result.size() >= 1, "");
    return result;
  }
};

}  // namespace split_polygon_polyline_detail

namespace {
constexpr auto const& binary_split_polygon_polyline
 = static_const<split_polygon_polyline_detail::binary_split_fn>::value;
}

namespace split_polygon_polyline_detail {

struct split_polygon_polyline_fn {
  /// Splits the polygon \p pg with the polyline \p pl.
  ///
  /// It:
  ///
  /// - 1. Clips the polyline with the polygon, such that only the parts of the
  ///   polyline that are inside the polygon remains (this results in a range of
  ///   polyline pieces inside the polygon, each having two intersection points
  ///   with the polygon).
  ///
  /// - 2. Splits the original polygon by recursively performing binary splits
  ///   against the polyline pieces.
  template <typename PG, typename PL, typename UPG = uncvref_t<PG>>
  inline auto operator()(PG&& pg, PL&& pl) const noexcept {
    static_assert(Polygon<UPG>{});
    static_assert(Polyline<uncvref_t<PL>>{});
    static_assert(ad_v<PG> == ad_v<PL>);

    small_vector<UPG, 3> result = {pg};

    suint_t pg_idx = 0;
    while (pg_idx < ranges::size(result)) {
    start:
      auto&& pg_         = result[pg_idx];
      auto[_, polylines] = intersection_polygon_polyline(pg_, pl);

      for (auto&& pl_ : polylines) {
        auto s = binary_split_polygon_polyline(pg_, pl_);
        if (ranges::size(s) == 1) { continue; }
        auto it = ranges::begin(result) + pg_idx;
        result.erase(it);
        result.insert(ranges::end(result), ranges::begin(s), ranges::end(s));

        goto start;
      }
      ++pg_idx;
    }
    return result;
  }
};

}  // namespace split_polygon_polyline_detail

namespace {
constexpr auto const& split_polygon_polyline = static_const<
 split_polygon_polyline_detail::split_polygon_polyline_fn>::value;
}

}  // namespace hm3::geometry
