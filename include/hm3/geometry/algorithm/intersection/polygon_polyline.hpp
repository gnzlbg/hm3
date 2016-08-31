#pragma once
/// \file
///
/// Intersection of a polygon witha polyline.
#include <hm3/geometry/algorithm/intersection.hpp>
#include <hm3/geometry/algorithm/intersection/polygon_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_segment.hpp>
#include <hm3/geometry/algorithm/split/polyline_points.hpp>
#include <hm3/geometry/primitive/polygon/as_polyline.hpp>
#include <hm3/geometry/primitive/polyline/as_polygon.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>

namespace hm3::geometry::polygon_primitive {

template <typename PolyG, typename PolyL, typename UPG = uncvref_t<PolyG>,
          typename UPL = uncvref_t<PolyL>, dim_t Nd = UPG::dimension(),
          CONCEPT_REQUIRES_(Nd == UPL::dimension() and Polygon<UPG, Nd>()
                            and Polyline<UPL, Nd>{})>
inline auto intersection(PolyG&& pg, PolyL&& pl) {
  using p_t  = point<Nd>;
  using s_t  = segment<Nd>;
  using pl_t = UPL;

  auto pg_pl = as_polyline<UPL>(pg);

  small_vector<p_t, 10> intersection_points;
  small_vector<pl_t, 4> intersection_poly_lines;

  auto push_back_ip
   = [&](auto&& p) { unique_push_back(intersection_points, p); };

  // Compute intersection points between polygon and polyline O(N_pg x N_pl x
  // N_ip)
  for (auto&& pg_s : faces(pg_pl)) {  // O(N_pg)
    for (auto&& pl_s : faces(pl)) {   // O(N_pl)
      auto i = geometry::intersection(pg_s, pl_s);
      visit(
       [&](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, p_t>{}) { push_back_ip(v); }  // O(N_ip)
         else if
           constexpr(Same<T, s_t>{}) {
             push_back_ip(v.x(0));  // O(N_ip)
             push_back_ip(v.x(1));  // O(N_ip)
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
  auto pl_pls = geometry::split(pl, intersection_points);

  // Remove polylines outside the polygon => range of polylines inside polygon
  {
    auto is_outside = [&pg](auto&& l) {
      // A polyline is outside if any of its vertices is outside
      // the polygon:
      return ranges::any_of(vertices(l), [&pg](auto&& vx) {
        return !geometry::intersection.test(pg, vx);
      });
    };

    ranges::action::remove_if(pl_pls, is_outside);
  }

  // Intersection points not part of any polyline are also part of the
  // intersection:
  small_vector<p_t, 2> points;
  for (auto&& ip : intersection_points) {
    if (ranges::none_of(pl_pls, [&ip](auto&& l) {
          return ranges::none_of(faces(l), [&ip](auto&& s) {
            return geometry::intersection.test(s, ip);
          });
        })) {
      unique_push_back(points, ip);
    }
  }

  return hm3::make_pair(points, pl_pls);
}

template <typename PolyG, typename PolyL, typename UPG = uncvref_t<PolyG>,
          typename UPL = uncvref_t<PolyL>, dim_t Nd = UPG::dimension(),
          CONCEPT_REQUIRES_(Nd == UPL::dimension() and Polygon<UPG, Nd>()
                            and Polyline<UPL, Nd>{})>
inline auto intersection_test(PolyG&& pg, PolyL&& pl) {
  auto r = geometry::intersection(pg, pl);
  return r.first().size() > 0 or r.second().size() > 0;
}

}  // namespace hm3::geometry::polygon_primitive
