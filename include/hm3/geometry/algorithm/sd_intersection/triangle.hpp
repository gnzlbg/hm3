#pragma once
/// \file
///
/// Intersection of triangles with signed-distance functions.
#include <hm3/ext/variant.hpp>
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/algorithm/concatenate.hpp>
#include <hm3/geometry/algorithm/edge.hpp>
#include <hm3/geometry/algorithm/sd_relative_position.hpp>
#include <hm3/geometry/algorithm/sd_sign.hpp>
#include <hm3/geometry/algorithm/vertex.hpp>
#include <hm3/geometry/concept/point.hpp>
#include <hm3/geometry/concept/polygon.hpp>
#include <hm3/geometry/concept/polyline.hpp>
#include <hm3/geometry/concept/segment.hpp>
#include <hm3/geometry/concept/signed_distance.hpp>
#include <hm3/math/interpolation/linear.hpp>
#include <hm3/utility/fixed_capacity_vector.hpp>

namespace hm3::geometry::sd {

namespace intersection_triangle_detail {

/// Result of an intersection between a signed-distance field and a triangle:
/// - empty
/// - point
/// - one line segment
/// - polyline (two edges)
/// - point and segment (one edge and opposite vertex)
/// - triangle (zero-level == triangle boundary)
template <typename T, typename P = associated::point_t<T>,
          typename S  = associated::edge_t<T>,
          typename PL = associated::polyline_t<T>>
using triangle_intersection_result
 = variant<monostate, P, S, PL, pair<P, S>, T>;

struct intersection_triangle_fn {
  /// Returns the intersection of a signed-distance function \p sdf with the
  /// triangle \p tri.
  template <typename T, typename SDF>
  constexpr auto operator()(T const& tri, SDF&& sdf, num_t abs_tol,
                            num_t rel_tol) const noexcept
   -> triangle_intersection_result<T> {
    using pl_t = associated::polyline_t<T>;
    using s_t  = associated::edge_t<T>;
    using p_t  = associated::point_t<T>;

    static_assert(Polygon<T>{});
    static_assert(SignedDistance<uncvref_t<SDF>, p_t>{});
    static_assert(ad_v<T> == ad_v<SDF>);
    HM3_ASSERT(vertex_size(tri) == 3, "the polygon {} is not a triangle!", tri);

    using math::signum_t;
    auto signum = [&](auto&& v) { return sign(v, abs_tol, rel_tol); };

    triangle_intersection_result<T> result;

    auto const& vxs = vertices(tri);

    // Utility to wrap around the last triangle vertex:
    auto next_vx = [&](auto&& i) { return (i != 2) ? i + 1 : 0; };

    // Compute signed-distance and signum at the triangle vertices:
    const array<num_t, 3> sd{{sdf(vxs[0]), sdf(vxs[1]), sdf(vxs[2])}};
    const array<signum_t, 3> sg{{signum(sd[0]), signum(sd[1]), signum(sd[2])}};

    /// If the triangle is not intersected, return nothing:
    if (relative_position(sg) != intersected) { return monostate{}; }

    // Count the number of vertices intersected by the zero-level:
    const auto no_cut_vertices = ranges::count(sg, signum_t::zero());

    // 99.99% of the time no vertices are intersected by the zero level:
    HM3_LIKELY(no_cut_vertices == 0);

    if (HM3_UNLIKELY(no_cut_vertices == 3)) {
      // Three vertices can be cut if:
      // - the sdf is a triangle that perfectly matches the edges of \p tri
      // - the sdf is corner-like, and it matches two edges of \p tri
      // - the sdf matches one edge, while the vertex not on that edge is cut by
      //   another zero level

      // These cases are disambiguated by evaluating the sdf at the edge
      // centroids:
      array<num_t, 3> centroid_sd;
      array<signum_t, 3> centroid_sg;
      for (auto eidx : edge.indices(tri)) {
        auto s            = edge(tri, eidx);
        auto sc           = centroid(s);
        centroid_sd[eidx] = sdf(sc);
        centroid_sg[eidx] = signum(centroid_sd[eidx]);
      }

      auto no_cut_edge_centers = ranges::count_if(
       centroid_sg, [](auto&& i) { return i == signum_t::zero(); });

      switch (no_cut_edge_centers) {
        case 3: {
          // If they are all zero the intersection is the triangle boundary:
          HM3_ASSERT(signum(sdf(centroid(tri))) != signum_t::zero(),
                     "Signed-distance function is not a signed-distance "
                     "field! It is zero at all vertices of the triangle as "
                     "well as in the triangle centroid!");
          return tri;
        }
        case 2: {
          // Two edges are zero, the boundary is a polyline:
          fixed_capacity_vector<pl_t, 2> segments;
          for (auto eidx : edge.indices(tri)) {
            if (centroid_sg[eidx] == signum_t::zero()) {
              segments.push_back(edges(edge(tri, eidx)));
            }
          }
          HM3_ASSERT(segments.size() == 2, "");
          return concatenate.direction_independent(segments[0], segments[1])
           .value();
        }
        case 1: {
          // One edge is intersected, that edge and the other vertex are the
          // boundary. Add the segment:
          s_t s;
          for (auto eidx : edge.indices(tri)) {
            if (centroid_sg[eidx] == signum_t::zero()) {
              s = edge(tri, eidx);
              break;
            }
          }
          // Find vertex not part of the segment and return pair:
          for (auto&& v : vxs) {
            if (v != s.x(0) and v != s.x(1)) { return make_pair(v, s); }
          }
          HM3_UNREACHABLE();
        }
        default: { HM3_UNREACHABLE(); }
      }
    }
    HM3_ASSERT(no_cut_vertices != 3,
               "");  // this should have been handled above

    // If two vertices are intersected by the zero level, we assume that
    // one edge of the triangle corresponds to the boundary:
    if (HM3_UNLIKELY(no_cut_vertices == 2)) {
      for (auto&& i : vertex.indices(tri)) {
        auto ni = next_vx(i);
        if (sg[i] == signum_t::zero() and sg[ni] == signum_t::zero()) {
          return s_t(vxs[i], vxs[ni]);
        }
      }
    }

    HM3_ASSERT(no_cut_vertices < 2, "");  // this should have been handled above

    HM3_ASSERT(
     !ranges::all_of(sg, [](auto i) { return i != signum_t::negative(); })
      or !ranges::all_of(sg, [](auto i) { return i != signum_t::positive(); }),
     "if the vertices are all positive or negative there is no intersection");

    // Vertices of different signs (or one is zero), the result is always a
    // segment.
    //
    // Find the segment cut points:
    fixed_capacity_vector<p_t, 2> cut_points;
    for (auto vidx : vertex.indices(tri)) {
      const auto n_vidx     = next_vx(vidx);
      const auto vx_sd      = sd[vidx];
      const auto next_vx_sd = sd[n_vidx];
      const auto vx_sg      = sg[vidx];
      const auto next_vx_sg = sg[n_vidx];

      // If the sign of the next vertex:
      // - is the same, this edge is not intersected,
      // - is zero, this edge will be handled in the next iteration.
      if (vx_sg == next_vx_sg or next_vx_sg == signum_t::zero()) { continue; }

      // If the sign of this vertex is zero, add it to the cut points
      if (vx_sg == signum_t::zero()) {
        cut_points.push_back(vertex(tri, vidx));
        continue;
      }

      // Otherwise this edge is intersected by the zero level-set.
      // The point in the edge with value 0 is found by linear interpolation:
      auto x_cp = math::ip::linear::point_with_value(0., vxs[vidx], vxs[n_vidx],
                                                     vx_sd, next_vx_sd);
      cut_points.push_back(x_cp);
      // and we move to the next edge.
    }

    if (cut_points.size() == 1) { return cut_points[0]; }

    HM3_ASSERT(cut_points.size() == 2, "#of cut points is not 2: {}",
               cut_points.size());

    return s_t(cut_points[0], cut_points[1]);
  }
};

}  // namespace intersection_triangle_detail

namespace {
constexpr auto const& intersection_triangle
 = static_const<with_default_tolerance<
  intersection_triangle_detail::intersection_triangle_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
