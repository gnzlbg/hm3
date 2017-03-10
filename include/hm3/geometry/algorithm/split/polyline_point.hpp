#pragma once
/// \file
///
/// Split polyline at a point.
#include <hm3/geometry/algorithm/approx/point.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/utility/fixed_capacity_vector.hpp>

namespace hm3::geometry {

namespace split_polyline_point_detail {

struct split_polyline_point_fn {
  /// Splits the polyline \p pl at the point \p p.
  template <typename P, typename PL, typename UPL = uncvref_t<PL>>
  constexpr fixed_capacity_vector<UPL, 2> operator()(PL&& pl, P const& p) const
   noexcept {
    static_assert(Point<P>{});
    static_assert(Polyline<UPL>{});
    static_assert(ad_v<P> == ad_v<UPL>);
    using r_t = fixed_capacity_vector<UPL, 2>;

    auto edges_before = [&](auto v) { return edges(pl) | view::take(v); };
    auto edges_after  = [&](auto v) { return edges(pl) | view::drop(v); };

    // Splits a polylina at a vertex
    auto split_at_vertex = [&](suint_t svidx) {
      const auto no_vids = vertex_size(pl);
      HM3_ASSERT(svidx < no_vids, "vertex out-of-bounds [0, {})", no_vids);
      // If it's the first or last vertex, the result is the whole polyline:
      if (svidx == 0 or svidx == no_vids - 1) { return r_t{pl}; }
      // Otherwise, the result is two adjacent polylines sharing vertex \p
      // svidx:
      return r_t{UPL(edges_before(svidx)), UPL(edges_after(svidx))};
    };

    for (auto&& sidx : edge.indices(pl)) {
      auto&& s   = edge(pl, sidx);
      auto ir    = intersection_segment_point(s, p);
      r_t result = visit(
       [&](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, P>{}) {
             if (geometry::approx(s.x(0), p)) { return split_at_vertex(sidx); }
             if (geometry::approx(s.x(1), p)) {
               return split_at_vertex(sidx + 1);
             }
             auto el = s;
             el.x(1) = v;

             auto er = s;
             er.x(0) = v;

             auto edges_l = view::concat(edges_before(sidx), view::single(el));
             auto edges_r
              = view::concat(view::single(er), edges_after(sidx + 1));
             return r_t{UPL(edges_l), UPL(edges_r)};
           }
         else if
           constexpr(Same<T, monostate>{}) { return r_t{}; }
         else {
           HM3_STATIC_ASSERT_EXHAUSTIVE_VISITOR(T);
         }
       },
       ir);
      if (not result.empty()) { return result; }
    }
    return {};
  }
};

}  // namespace split_polyline_point_detail

namespace {
constexpr auto const& split_polyline_point
 = static_const<split_polyline_point_detail::split_polyline_point_fn>::value;
}

}  // namespace hm3::geometry
