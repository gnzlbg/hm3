#pragma once
/// \file
///
/// Intersection of polygons with a signed distance function
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/sd/concepts.hpp>
#include <hm3/interpolation/linear.hpp>
#include <hm3/utility/inline_vector.hpp>

namespace hm3::geometry::polygon_primitive {

namespace sd_detail {

/// Result of a triangle intersection:
/// - shapes in the positive and negative regions of the signed-distance field
/// (inside/outside)
/// - boundary segments of the zero-th level
template <dim_t Nd>
struct triangle_intersection_result {
  struct shape_t {
    shape_t()               = default;
    shape_t(shape_t const&) = default;
    shape_t(shape_t&&)      = default;
    shape_t& operator=(shape_t const&) = default;
    shape_t& operator=(shape_t&&) = default;
    template <typename Vs, typename Sgs>
    shape_t(Vs&& vs, Sgs&& sgs)
     : vertices(std::forward<Vs>(vs)), signum(std::forward<Sgs>(sgs)) {}
    // A linear split produces at most a quad:
    bounded_polygon<Nd, 4> vertices;
    inline_vector<math::signum_t, 4> signum;
  };

  inline_vector<shape_t, 1> inside, outside;
  // Three boundary segments are possible (e.g. for a sd-field that is
  // triangle-like and matches with the triangle):
  inline_vector<segment<Nd>, 3> boundary;
};

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& os, triangle_intersection_result<Nd> const& r) {
  os << "[intersection result:\n";
  os << "  inside polygon: ";
  if (r.inside.size() == 0) {
    os << "-";
  } else {
    os << "\n    vx: " << r.inside[0].vertices;
    os << "\n    sg: " << view::all(r.inside[0].signum);
  }
  os << "\n";
  os << "  outside polygon: ";
  if (r.outside.size() == 0) {
    os << "-";
  } else {
    os << "\n    vx: " << r.outside[0].vertices;
    os << "\n    sg: " << view::all(r.outside[0].signum);
  }
  os << "\n";
  os << "  boundary: ";
  if (r.boundary.size() == 0) {
    os << "-";
  } else {
    os << view::all(r.boundary);
  }
  os << "\n]";
  return os;
}

/// Intersects a signed-distance function \p sdf with the triangle \p tri and
/// returns the shapes of the polygons in the posotive and negative side of the
/// signed-distance field as well as the boundary located at the zero-th level.
/// The boundary normals always point in the direction of the positive side of
/// the signed-distance field.
///
/// This function tries really hard to produce meaningful results even
/// when the signed-distance function at the vertices is exactly zero.
template <dim_t Nd, typename SDF, CONCEPT_REQUIRES_(SignedDistance<SDF, Nd>{})>
triangle_intersection_result<Nd> signed_distance_intersection_triangle(
 triangle<Nd> const& tri, SDF&& sdf) {
  using vidx_t = access::vertex_index_t<triangle<Nd>>;
  using bp4    = bounded_polygon<Nd, 4>;
  using math::signum;
  using math::signum_t;

  triangle_intersection_result<Nd> result;
  auto const& vxs = vertices(tri);

  // Utility to wrap around the last triangle vertex:
  auto next_vx = [&](auto&& i) { return (i != 2) ? i + 1 : 0; };

  // Compute signed-distance and signum at the triangle vertices:
  const array<num_t, 3> sd{{sdf(vxs[0]), sdf(vxs[1]), sdf(vxs[2])}};
  const array<signum_t, 3> sg{{signum(sd[0]), signum(sd[1]), signum(sd[2])}};

  // Count the number of vertices intersected by the zero-level:
  const auto no_cut_vertices = ranges::count(sg, signum_t::zero());

  // The case that happens 100% of the time is that no vertices are intersected
  // by the zero level of the signed-distance field:
  HM3_LIKELY(no_cut_vertices == 0);

  // TODO: these cases are found as edge cases during testing, but hopefully
  // they never ever occur in practice (and if they do occur you have bigger
  // problems). Trying to solve them probably doesn't add any value.
  //
  // If all vertices are cut by the zero level it's almost game over, but we can
  // easily do two things to try to save the day:
  //
  // First, we can try to find a triangle edge whose centroid is not part of the
  // zero level. Depending on the sign of this centroid the triangle is inside
  // or outside, and by eliminating that edge from the boundary we get a
  // meaningful result in the case of a corner-like signed-distance function
  // whose zero level perfectly overlaps two edges:
  //
  //      phi0
  //   |  |
  //   |
  //   |  *\    phi at all 3 vertices is zero, but at the centroid x it is not
  //   |  | \x
  //   |  |  \
  //   |  *---* -------- phi0
  //   |_________ phi1
  //
  //
  // If the more than one edge's centroid's signed-distance value is not zero
  // there is nothing we can do (maybe try a different mesh). That is really
  // game over.
  //
  // Second, consider a triangle-like signed distance field. The edges of the
  // triangle are the boundary. In this case we can compute the signed-distance
  // function of the triangle's centroid to determine whether the triangle is
  // inside or outside.
  if (HM3_UNLIKELY(no_cut_vertices == 3)) {
    // Compute the edge's centroid, evaluate the sdf there, and add those with
    // signum zero to the boundary:
    array<num_t, 3> centroid_sd;
    array<signum_t, 3> centroid_sg;
    for (auto fidx : face_indices(tri)) {
      auto s            = face(tri, fidx);
      auto sc           = centroid(s);
      centroid_sd[fidx] = sdf(sc);
      centroid_sg[fidx] = signum(centroid_sd[fidx]);
      if (centroid_sg[fidx] == signum_t::zero()) {
        result.boundary.push_back(s);
      }
    }
    // If they are all zero try to handle the triangle-like sd-field by using
    // the value at the triangles centroid to detect whether the triangle is
    // inside or outside.
    if (ranges::all_of(centroid_sg, [](auto&& i) { return i == 0; })) {
      auto sdc = sdf(centroid(tri));
      auto sgc = signum(sdc);
      if (sgc == signum_t::zero()) {
        HM3_FATAL_ERROR("Signed-distance function is not a signed-distance "
                        "field! It is zero at all vertices of the triangle as "
                        "well as in the triangle centroid!");
      }

      if (sgc == signum_t::positive()) { result.inside.emplace_back(tri, sg); }
      if (sgc == signum_t::negative()) {
        result.outside.emplace_back(tri, sg);
        // centroid is negative => boundary normals point in the wrong dir:
        for (auto&& b : result.boundary) { b = direction.invert(b); }
      }
      return result;
    }
    // If more than one edge's centroid is not zero nothing can be done:
    if (ranges::count_if(centroid_sg,
                         [](auto&& i) { return i != signum_t::zero(); })
        > 1) {
      HM3_FATAL_ERROR("Probably broken signed-distance field: all vertices of "
                      "a trinagle are cut by the zero-level, but more than one "
                      "edge's centroid is not on the zero level =/");
    }
    // Otherwise detect inside/outside and add the triangle itself as shape:
    if (ranges::any_of(centroid_sg,
                       [](auto&& i) { return i == signum_t::positive(); })) {
      result.inside.emplace_back(tri, sg);
      return result;
    }
    if (ranges::any_of(centroid_sg,
                       [](auto&& i) { return i == signum_t::negative(); })) {
      result.outside.emplace_back(tri, sg);
      // tri is negative => boundary normals point in the wrong dir:
      for (auto&& b : result.boundary) { b = direction.invert(b); }
      return result;
    }
    HM3_UNREACHABLE();  // this should handle all no_cut_vertices == 3 cases
  }
  HM3_ASSERT(no_cut_vertices != 3, "");  // this should have been handled above

  // If two vertices are intersected by the zero level, we assume that
  // one edge of the triangle corresponds to the boundary:
  if (HM3_UNLIKELY(no_cut_vertices == 2)) {
    for (auto&& i : vertex_indices(tri)) {
      auto ni = next_vx(i);
      if (sg[i] == signum_t::zero() and sg[ni] == signum_t::zero()) {
        result.boundary.emplace_back(segment<Nd>({vxs[i], vxs[ni]}));
        break;
      }
    }
  }

  // If all vertices inside we are done, the boundary, if any, has already been
  // added:
  if (ranges::all_of(sg, [](auto i) { return i != signum_t::negative(); })) {
    result.inside.emplace_back(tri, sg);
    return result;
  }
  // If all vertices outside we are (almost) done. The boundary, if any, has
  // already been added, but it needs to be flipped such that its normal
  // points in the positive side of the signed-distance field:
  if (ranges::all_of(sg, [](auto i) { return i != signum_t::positive(); })) {
    result.outside.emplace_back(tri, sg);
    if (result.boundary.size() > 0) {
      result.boundary[0] = direction.invert(result.boundary[0]);
    }
    return result;
  }
  HM3_ASSERT(no_cut_vertices != 2, "");  // the two if should have handled this
  HM3_ASSERT(no_cut_vertices == 0 or no_cut_vertices == 1, "");

  // From here till the end, we have some vertices of different signs, so the
  // intersection will have a single boundary, some part inside, and some part
  // outside:
  result.inside.push_back();
  result.outside.push_back();
  result.boundary.push_back();

  inline_vector<point<Nd>, 2> boundary_cut_points;
  /// Adds vertices and cut points to each polygon
  for (auto vidx : vertex_indices(tri)) {
    const auto n_vidx     = next_vx(vidx);
    const auto vx_sd      = sd[vidx];
    const auto next_vx_sd = sd[n_vidx];
    const auto vx_sg      = sg[vidx];
    const auto next_vx_sg = sg[n_vidx];

    // If a vertex is cut by the zero level set it is part of both
    // polygons (inside and outside) and the cut surface, otherwise it is only
    // part of one polygon
    if (vx_sg == signum_t::zero()) {
      result.inside[0].vertices.push_back(vxs[vidx]);
      result.outside[0].vertices.push_back(vxs[vidx]);
      result.inside[0].signum.push_back(vx_sg);
      result.outside[0].signum.push_back(vx_sg);
      boundary_cut_points.push_back(vxs[vidx]);
      continue;  // we are then done since we found the cut points
    } else if (vx_sg == signum_t::positive()) {
      result.inside[0].vertices.push_back(vxs[vidx]);
      result.inside[0].signum.push_back(vx_sg);
    } else if (vx_sg == signum_t::negative()) {
      result.outside[0].vertices.push_back(vxs[vidx]);
      result.outside[0].signum.push_back(vx_sg);
    } else {
      HM3_UNREACHABLE();
    }

    // If:
    // - the sign of the next vertex is the same, this edge is not
    //   intersected, so we just move on to the next edge.
    // - the sign of the next vertex is zero, it will be handled in the next
    //   iteration.
    if (vx_sg == next_vx_sg || next_vx_sg == signum_t::zero()) { continue; }

    // Otherwise this edge is intersected by the zero level-set.
    // The point in the edge with value 0 is found by linear interpolation:
    auto x_cp = ip::linear::point_with_value(0., vxs[vidx], vxs[n_vidx], vx_sd,
                                             next_vx_sd);

    // Cut points are part fo both polygons and part of the surface:
    result.inside[0].vertices.push_back(x_cp);
    result.outside[0].vertices.push_back(x_cp);
    result.inside[0].signum.push_back(0);
    result.outside[0].signum.push_back(0);
    boundary_cut_points.push_back(x_cp);

    // and we move to the next edge.
  }

  HM3_ASSERT(!ranges::all_of(result.inside[0].signum,
                             [](auto&& i) { return i == signum_t::zero(); }),
             "");
  HM3_ASSERT(!ranges::all_of(result.outside[0].signum,
                             [](auto&& i) { return i == signum_t::zero(); }),
             "");
  HM3_ASSERT(boundary_cut_points.size() == 2, "");

  // Create the boundary (where the signed distance function is zero):
  if (HM3_LIKELY(no_cut_vertices == 0)) {
    HM3_ASSERT(
     result.inside[0].vertices.size() != result.outside[0].vertices.size(), "");

    // If no triangle vertices are cut by the zero level, and the inside polygon
    // has more vertices than the outside one, then the order is correct,
    // otherwise we need to flip it:
    if (result.inside[0].vertices.size() > result.outside[0].vertices.size()) {
      result.boundary[0] = segment<Nd>(boundary_cut_points);
    } else {
      result.boundary[0] = segment<Nd>(boundary_cut_points | view::reverse);
    }
  } else {
    HM3_ASSERT(no_cut_vertices == 1, "");
    HM3_ASSERT(
     result.inside[0].vertices.size() == result.outside[0].vertices.size(), "");
    // If one triangle vertex is cut by the zero level, then the inside and
    // outside polygons are triangles (the cases where there is just one polygon
    // has been covered above already).
    //
    // The normal is correct if the vertex after the one with signum zero has
    // negative signum
    for (auto&& i : vertex_indices(tri)) {
      if (sg[i] != signum_t::zero()) { continue; }
      // i is the vertex with signum zero and j is the next vertex
      auto j = next_vx(i);
      if (sg[j] == signum_t::negative()) {  // then the order is correct
        result.boundary[0] = segment<Nd>(boundary_cut_points);
        break;
      }
      // otherwise we need to flip the normal:
      result.boundary[0] = segment<Nd>(boundary_cut_points | view::reverse);
      break;
    }
  }

  // Check that the result are closed polygons (not a single point or a line)
  HM3_ASSERT(result.inside[0].vertices.size() > 2
              || result.inside[0].vertices.size() == 0,
             "shape0 size: {} ", result.inside[0].vertices.size());
  HM3_ASSERT(result.outside[0].vertices.size() > 2
              || result.outside[0].vertices.size() == 0,
             "shape1 size: {}", result.outside[0].vertices.size());

  return result;
}

}  // namespace sd_detail

}  // namespace hm3::geometry::polygon_primitive
