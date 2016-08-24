#pragma once
/// \file
///
/// Split polyline at a point.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3::geometry::point_primitive {

/// Splits the polyline \p pl at the point \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polyline<P, Nd>{})>
constexpr variant<monostate, UP, pair<UP, UP>> split(P&& pl,
                                                     point<Nd> const& p) {
  using p_t = point<Nd>;
  using r_t = variant<monostate, UP, pair<UP, UP>>;

  // Splits a polylina at a vertex
  auto split_polyline_at_vertex = [](auto&& pl_, suint_t svidx) {
    const auto no_vids = vertex_size(pl_);
    // If it's the first or last vertex, the result is the whole polyline:
    if (svidx == 0 or svidx == no_vids - 1) { return r_t{pl_}; }
    // Otherwise, the result is two adjacent polylines sharing vertex \p svidx:
    UP pl_l;
    UP pl_r;
    for (suint_t vidx = 0; vidx <= svidx; ++vidx) {
      pl_l.push_back(vertex(pl_, vidx));
    }
    for (suint_t vidx = svidx; vidx < no_vids; ++vidx) {
      pl_r.push_back(vertex(pl_, vidx));
    }
    return r_t{make_pair(pl_l, pl_r)};
  };

  for (auto&& sidx : face_indices(pl)) {
    auto&& s = face(pl, sidx);
    auto ir  = intersection(s, p);
    auto r   = visit(
     [&](auto&& v) {
       using T = uncvref_t<decltype(v)>;
       if
         constexpr(Same<T, p_t>{}) {
           if (distance.approx(s.x(0), p)) {
             return split_polyline_at_vertex(pl, sidx);
           }
           if (distance.approx(s.x(1), p)) {
             return split_polyline_at_vertex(pl, sidx + 1);
           }
           insert_vertex(pl, sidx, v);
           return split_polyline_at_vertex(pl, sidx + 1);
         }
       else if
         constexpr(Same<T, monostate>{}) { return r_t{monostate{}}; }
       else {
         static_assert(fail<T>{}, "non-exhaustive visitor");
       }
     },
     ir);
    if (r.index() != 0) { return r; }
  }
  return monostate{};
}

}  // namespace hm3::geometry::point_primitive
