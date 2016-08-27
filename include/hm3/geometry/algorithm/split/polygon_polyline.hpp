#pragma once
/// \file
///
/// Split polygon at a polyline.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/split/polyline_points.hpp>
#include <hm3/geometry/primitive/polygon/as_polyline.hpp>
#include <hm3/geometry/primitive/polyline/as_polygon.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/variant.hpp>
#include <hm3/utility/vector.hpp>

namespace hm3::geometry::split_detail {

namespace detail {

/// Splits the polygon \p pg with the polyline \p pl into one or two polygons.
///
/// \pre The number of intersection points between segments of the polyline \p
/// pl and the polygon \pg is at most two.
///
/// \pre Only the first and last segment of the polyline might intersect the
/// polygon.
template <typename PolyG, typename PolyL, typename UPG = uncvref_t<PolyG>,
          typename UPL = uncvref_t<PolyL>, dim_t Nd = UPG::dimension(),
          CONCEPT_REQUIRES_(Nd == UPL::dimension() and Polygon<UPG, Nd>()
                            and Polyline<UPL, Nd>{})>
constexpr inline_vector<UPG, 2> binary_split(PolyG&& pg, PolyL&& pl) {
  using p_t = point<Nd>;
  using s_t = segment<Nd>;

  inline_vector<UPG, 2> result;
  small_vector<p_t, 8> intersection_points;
  small_vector<p_t, 8> intersection_points_segments;

  auto push_back_ip = [&](auto&& p) {
    if (ranges::end(intersection_points)
        == ranges::find(intersection_points, p)) {
      // HM3_ASSERT(intersection_points.size() < intersection_points.capacity(),
      //            "ips: {}, np: {}", view::all(intersection_points), p);
      intersection_points.push_back(p);
    }
  };

  // Appends the intersection points of a polygon segment with a polyline
  // segment to the intersection_points vector:
  auto append_intersection_points = [&](auto&& pg_segment, auto&& pl_segment) {
    auto i = intersection(pg_segment, pl_segment);
    visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, p_t>{}) { push_back_ip(v); }
       else if
         constexpr(Same<T, s_t>{}) {
           intersection_points_segments.push_back(v.x(0));
           intersection_points_segments.push_back(v.x(1));
         }
       else if
         constexpr(Same<T, monostate>{}) {}
       else {
         static_assert(fail<T>{}, "non-exhaustive visitor");
       }
     },
     i);
  };

  const auto no_pl_segments = face_size(pl);

  auto first_pl_segment = face(pl, 0);
  auto last_pl_segment  = face(pl, no_pl_segments - 1);
  for (auto&& pg_segment : faces(pg)) {  // O(N_pgs)
    append_intersection_points(pg_segment, first_pl_segment);
    if (no_pl_segments == 1) { continue; }
    HM3_ASSERT(first_pl_segment != last_pl_segment, "");
    append_intersection_points(pg_segment, last_pl_segment);
  }

  // Remove all intersection points that belong to an intersected segment
  intersection_points.erase(
   ranges::remove_if(intersection_points,
                     [&](auto&& v) {
                       return end(intersection_points_segments)
                              != ranges::find(intersection_points_segments, v);
                     }),
   end(intersection_points));

  HM3_ASSERT(intersection_points.size() < 3, "max of 2 intersection points");

  if (intersection_points.size() < 2) {
    // for zero or one intersection (e.g. at a corner point), the resulting
    // polygon is the input polygon:
    result.push_back(pg);
    return result;
  }

  // From now on two intersection points:
  auto pg_pl = as_polyline<UPL>(pg);

  // Split the polygon at the two intersection points into three polylines
  // vx0 ----- ip0 ----- ip1 ------ vx0
  auto pg_pls = split(pg_pl, intersection_points);
  HM3_ASSERT(pg_pls.size() == 3, "");

  // Merge the first and the last polylines of the split (they are part of the
  // same polygon):
  auto pg0_pl_ = set_union(pg_pls[2], pg_pls[0]);
  HM3_ASSERT(pg0_pl_, "??");
  auto pg0_pl = pg0_pl_.value();
  // The second polyline of the split is part of the other polygon:
  auto pg1_pl = pg_pls[1];

  // Merge the polylines of each polygon with the polyline used to perform the
  // split => two full polygons (as polylines):
  auto r0 = set_union(pg0_pl, pl);
  auto r1 = set_union(pg1_pl, pl);

  // For one of the polygons the polyline of the split is going to have the
  // wrong direction. Check here if creating the polygon succeeded and if not
  // invert the direction of the polyline and try again
  if (!r0) { r0 = set_union(pg0_pl, direction.invert(pl)); }
  if (!r1) { r1 = set_union(pg1_pl, direction.invert(pl)); }

  // We must have two fully formed polygons by now:
  HM3_ASSERT(r0, "??");
  HM3_ASSERT(r1, "??");

  // So it is safe to unwrap the values:
  auto r0_v = r0.value();
  auto r1_v = r1.value();

  // Finally since the resulting polygons are stored in a polyline we need
  // to convert them into a polygon:
  result.push_back(as_polygon<UPG>(r0_v));
  result.push_back(as_polygon<UPG>(r1_v));

  HM3_ASSERT(vertex_order.counter_clockwise(result[0]), "");
  HM3_ASSERT(vertex_order.counter_clockwise(result[1]), "");

  return result;
}

}  // namespace detail

/// Splits the polygon \p pg with the polyline \p pl.
///
/// In a nutshell this:
///
/// - clips the polyline with the polygon, such that only the parts of the
///   polyline that are inside the polygon remains (this results in a range of
///   polyline pieces inside the polygon)
///
/// - splits the original polygon by recursively performing binary splits
/// - against the polyline pieces.
template <typename PolyG, typename PolyL, typename UPG = uncvref_t<PolyG>,
          typename UPL = uncvref_t<PolyL>, dim_t Nd = UPG::dimension(),
          typename Ret = vector<UPG> /* small_vector<UPG, 3> */,
          CONCEPT_REQUIRES_(Nd == UPL::dimension() and Polygon<UPG, Nd>()
                            and Polyline<UPL, Nd>{})>
inline Ret split(PolyG&& pg, PolyL&& pl) {
  using p_t = point<Nd>;
  using s_t = segment<Nd>;
  Ret result;
  auto pg_pl = as_polyline<UPL>(pg);

  small_vector<p_t, 10> intersection_points;

  auto push_back_ip = [&](auto&& p) {
    if (ranges::end(intersection_points)
        == ranges::find(intersection_points, p)) {
      intersection_points.push_back(p);
    }
  };

  // Compute intersection points between polygon and polyline O(N_pg x N_pl)
  for (auto&& pg_s : faces(pg_pl)) {  // O(N_pg)
    for (auto&& pl_s : faces(pl)) {   // O(N_pl)
      auto i = intersection(pg_s, pl_s);
      visit(
       [&](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, p_t>{}) { push_back_ip(v); }
         else if
           constexpr(Same<T, s_t>{}) {
             push_back_ip(v.x(0));
             push_back_ip(v.x(1));
           }
         else if
           constexpr(Same<T, monostate>{}) {}
         else {
           static_assert(fail<T>{}, "non-exhaustive visitor");
         }
       },
       i);
    }
  }

  // Split polyline at the intersection points => range of polylines
  auto pl_pls = split(pl, intersection_points);

  // Remove polylines outside the polygon => range of polylines inside polygon
  {
    auto is_outside = [&pg](auto&& l) {
      // A polyline is outside if any of its vertices is outside
      // the polygon:
      return ranges::any_of(
       vertices(l), [&pg](auto&& vx) { return !intersection.test(pg, vx); });
    };

    pl_pls.erase(ranges::remove_if(pl_pls, is_outside), end(pl_pls));
  }

  // We split the polygon against each polyline
  result.push_back(pg);
  for (auto&& ls : pl_pls) {
    for (auto &&it = begin(result), e = end(result); it != e; ++it) {
      // Split the current polygon with the first polyline that works
      auto r = detail::binary_split(*it, ls);

      // If the result of the split is zero or one polygon, the poly line does
      // not split the current polygon so go to the next one:
      if (r.size() < 2_su) { continue; }

      // If the split succeeded, replace the split polygon with the two
      // resulting polygons:
      result.erase(it);
      result.push_back(r[0]);
      result.push_back(r[1]);
      // And go to the next polyline
      break;
    }
  }
  return result;
}

}  // namespace hm3::geometry::split_detail
