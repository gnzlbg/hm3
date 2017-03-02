#ifdef ABCD
#pragma once
/// \file
///
/// Split a polygon with a signed-distance field.
#include <hm3/geometry/primitive/polygon.hpp>

#include <hm3/geometry/algorithm/sd_intersection/polygon.hpp>
#include <hm3/geometry/algorithm/sd_split.hpp>
#include <hm3/geometry/algorithm/split/polygon_polyline.hpp>

namespace hm3::geometry::polygon_primitive {

template <typename P, typename SDF, dim_t Nd = uncvref_t<P>::dimension()>
vector<small_polygon<Nd, 4>> sd_split(P&& poly, SDF&& sdf) {
  if (geometry::sd_intersection.test(poly, sdf)
      != relative_position_t::intersected) {
    return {};
  }

  vector<small_polygon<Nd, 4>> result;

  auto ir = geometry::sd_intersection(poly, sdf);

  visit(
   [&](auto&& v) {
     using T = uncvref_t<decltype(v)>;

     if
       constexpr(Same<T, monostate>{}) {
         HM3_FATAL_ERROR("sd_intersection.test should have caught this above");
       }
     else if
       constexpr(Same<T, sd_intersection_result_t<Nd>>{}) {
         // Add the polygon to the result:
         result.push_back(poly);
         if (v.polylines.size() == 0) {
           HM3_ASSERT(v.points.size() > 0, "?");
           return;  // only cut at the corner points
         }

         // split the polygon (and its descendents) against all polylines
         for (auto&& pl : v.polylines) {
           suint_t i = 0;
           suint_t e = result.size();
           while (i < e) {
             auto sr = geometry::split(result[i], pl);
             if (sr.size() < 2) {
               HM3_ASSERT(sr.size() == 0
                           or (sr.size() == 1 and sr[0] == result[i]),
                          "??");
               ++i;
               continue;
             }

             result.erase(ranges::begin(result) + i);
             result.insert(ranges::end(result), ranges::begin(sr),
                           ranges::end(sr));
             --e;
           }
         }
       }
     else {
       static_assert(always_false<T>{}, "non-exhaustive visitor");
     }
   },
   ir);

  return result;
}

}  // namespace hm3::geometry::polygon_primitive
#endif
