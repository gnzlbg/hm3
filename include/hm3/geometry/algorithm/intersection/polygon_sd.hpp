#pragma once
/// \file
///
/// Intersection of polygons with a signed distance function
// #include <hm3/geometry/concepts.hpp>
// #include <hm3/geometry/primitive/point.hpp>
#include <hm3/geometry/primitive/polygon/intersection_sd_triangle.hpp>
// #include <hm3/geometry/sd/concept.hpp>
// #include <hm3/interpolation/linear.hpp>
// #include <hm3/utility/inline_vector.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

// todos: the faces of a segment are its corner points
// the faces of a polygon are its segments
// the faces of a polyhedra are its polygons

namespace sd_detail {
  /*
template <dim_t Nd>
triangle<Nd> triangle_from_segment_and_point(segment<Nd> const& s,
                                             point<Nd> const& p) {
  if (Nd == 3) { return triangle<Nd>(s.x(0), s.x(1), p); }
  triangle<Nd> t(s.x(0), s.x(1), p);
  auto n = normal(t);
  if (n(2) > 0.) { return t; }
  return triangle<Nd>(s.x(1), s.x(0), p);
}

template <typename TriIntRng>
polygon_intersection_result<Nd>
 merge_triangle_intersections_to_polygon_intersection(TriIntRng&& tis) {
  auto no_ints = size(tis);

  // find boundaries and number of regions:
  auto boundaries = bool_vector(no_ints)(no_ints);
  {  // find shared boundaries:
    auto boundary_matrix = bool_vector(no_ints)(no_ints);

    // for each triangle intersection:
    // - if tri has a boundary markt it with one, else zero

    // for each triangle i
    //   - if it has boundary i == i => 1 else continue,
    //   - boundary bi
    //   for each triangle j > i:
    //     - if j doesn't have boundary mark it with zero
    //     - if j has boundary and the end points match => put one, else zero

    // bi = 0
    // for each triangle i:
    // if m[i][i] == 1:
    //   if bi == 0: bi = i, push new boundary, push bi, m[i][i] = 0
    //   for each  j > i:
    //   if m[i][j] == 1, bj = j, push bj to bi boundary, m[j][i] = 0
    //
  }

  auto no_boundaries = boundaries.size();
  // if (no_boundaries > 1) { = > split cell ! }

  // what happens if the boundary is aligned with  one side of the polygon?
  auto no_regions = no_boundaries + 1;

  auto regions = vector(no_regions)(no_ints);

  {  // find regions
    auto inside_inside_neighbors   = bool_vector(no_ints)(no_ints);
    auto outside_outside_neighbors = bool_vector(no_ints)(no_ints);

    // for each inside region, find all other inside regions sharing a face

    // for each outside regin, find all other outside regions sharing a face

    // push the regions

    // push the regions sign (assert that al polygons of a region have the same
    // sign)
  }

  // so here I have for each boundary, which triangle segment it is formed from
  // and for each region (multiple inside, multiple outside) which triangle
  // inside/outside regions they are formed from

  // the boundaries are done, they are just the sgments and can be pushed to the
  // result

  // the regions need to be merged into  single polygons

  auto region_polygons(no_regions);
  auto region_signs(no_regions);

  // for each region:
  //  - region_polygons.push_back(merge_polygons(region.polys))

  return result;
}

// note the polygon cannot have holes (there are no equal segments)
template <typename P, dim_t Nd = uncvref_t<P>::dimension(), typename SDF,
          CONCEPT_REQUIRES_(SignedDistance<SDF, Nd>{} and Polygon<P, Nd>{})>
polygon_intersection_result<Nd> signed_distance_intersection_polygon(
 P&& poly, SDF&& sdf) {
  // cache corner signs
  // if all signs are equal => return empty intersection

  if (vertex_size(poly) == 3) {
    return signed_distance_intersection_triangle(std::forward<P>(poly),
                                                 std::forward<SDF>(sdf));
  }
  HM3_ASSERT(vertex_size(poly) > 3);
  // HM3_ASSERT(polygon_has_no_holes(poly), "");

  if (vertex_size(poly) == 4) {
    if (not_split(poly, sdf)) {
      return signed_distance_intersection_quad(
       poly, sdf);  // use faster version for non-split quads
    }
  }

  // otherwise triangulate:

  triangle_container tris;
  // is_convex: all its interior angles are less than 180°.
  if (is_convex(poly)) {
    auto xc           = geometry::centroid(poly);
    auto triangle_rng = segments(poly) | view::transform([&](auto&& seg) {
                          return triangle_from_segment_and_point(seg, xc);
                        });
    for (auto&& t : triangle_rng) { tris.emplace_back(t); }
  } else {  // is concave
    HM3_FATAL_ERROR(
     "unimplemented: singed distance intersection of non-convex polygon");
    // TODO: to implement this one just needs to be able to triangulate a
    // non-convex polygon. The simplest solution would be to just use the ear
    // clipping algorithm.
  }

  intersection_container tri_intersections;
  for (auto&& t : tris) {
    tri_intersections.emplace_back(
     signed_distance_intersection_triangle(t, sdf));
  }
  return merge_triangle_intersections_to_polygon_intersection(
   tri_intersections);
}

void intersection_multi(poly, sdfs) {
  // compute intersection with each sdf

  // each sdf has a number representing its "id"

  // merge the inside region of all intersections

  // for each boundary of the intersection => add its id (0, ... N for each
  // sdf, -1 for part of the original shape)
}
  */
}  // namespace sd_detail

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3

/*
namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

namespace sd_detail {

/// Maximum number of resulting boundary surfaces after intersecting a
/// polygon with \p no_vertices with a signed-distance function.
constexpr dim_t max_no_boundary_surfaces(dim_t no_vertices) noexcept {
  switch (no_vertices) {
    case 3: {  // triangle
      return 1;
    }
    case 4: {  // quad
      return 2;
    }
    case 5: {
      return 2;
    }
    default: { HM3_FATAL_ERROR("case {} not implemented", no_vertices); }
  }
}

/// Maximum number of resulting shapes per signed-distance side (pos and neg)
/// after intersecting a polygon with \p no_vertices with a signed-distance
/// function.
constexpr dim_t max_no_shapes_per_side(dim_t no_vertices) noexcept {
  switch (no_vertices) {
    case 3: {  // triangle
      return 1;
    }
    case 4: {  // quad
      return 2;
    }
    case 5: {
      return 2;
    }
    default: { HM3_FATAL_ERROR("case {} not implemented", no_vertices); }
  }
}

/// Maximum number of vertices that a shape resulting from intersecting a
/// polygon with \p no_vertices with a signed-distance function can have.
constexpr dim_t max_no_shape_vertices(dim_t no_vertices) noexcept {
  switch (no_vertices) {
    case 3: {  // triangle
      return 4;
    }
    case 4: {  // quad
      return 6;
    }
    case 5: {
      return 6;
    }
    default: { HM3_FATAL_ERROR("case {} not implemented", no_vertices); }
  }
}

template <typename Shape>  //
struct result {
  static constexpr dim_t nd = Shape::dimension();
  static constexpr dim_t original_shape_vertex_capacity
   = vertex_capacity(Shape{});
  static constexpr dim_t boundary_surface_capacity
   = max_no_boundary_surfaces(original_shape_vertex_capacity);
  static constexpr dim_t shapes_per_side_capacity
   = max_no_shapes_per_side(original_shape_vertex_capacity);
  static constexpr dim_t shape_vertex_capacity
   = max_no_shape_vertices(original_shape_vertex_capacity);

  using shape_t    = bounded_polygon<nd, shape_vertex_capacity>;
  using shapes_t   = inline_vector<shape_t, shapes_per_side_capacity>;
  using boundary_t = inline_vector<segment<nd>, boundary_surface_capacity>;
  using signum_t   = inline_vector<sint_t, shape_vertex_capacity>;
  using signums_t  = inline_vector<signum_t, shapes_per_side_capacity>;

  shapes_t inside, outside;
  boundary_t boundary;
  signums_t signum_inside, signum_outside;
};

template <dim_t Nd>  //
struct tri_result {
  struct shape_t {
    bounded_polygon<Nd, 4> vertices;
    inline_vector<sint_t, 4> signum;
  };
  struct boundary_t {
    segment<Nd> segments;
  };

  inline_vector<shape_t, 1> inside, outside;
  inline_vector<boundary_t, 3> boundary;
};

/// Intersects a signed-distance function \p sdf with the triangle \p tri and
/// returns the shapes of the polygons in the posotive and negative side of
/// the signed-distance field as well as the boundary located at the zero-th
/// level.
///
/// This function tries really hard to produce meaningful results even
/// when the signed-distance function at the vertices is exactly zero.
template <dim_t Nd, typename SDF, CONCEPT_REQUIRES_(SignedDistance<SDF, Nd>{})>
tri_result<Nd> signed_distance_intersection_tri(triangle<Nd> const& tri,
                                                SDF&& sdf) {
  tri_result result;
  auto const& vxs = vertices(s);

  // Utility to wrap around the last triangle vertex:
  auto next_vx = [&](auto&& i) { return (i != 2) ? i + 1 : 0; };

  // Compute signed-distance and signum at the triangle vertices:
  const array<num_t, 3> sd{sdf(vxs[0]), sdf(vxs[1]), sdf(vxs[2])};
  const array<sint_t, 3> sg{math::signum(sd[0]), math::signum(sd[1]),
                            math::signum(sd[2])};

  // Count the number of vertices intersected by the zero-level:
  const auto no_cut_vertices = ranges::count(sg, 0);

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
  if (no_cut_vertices == 3) {
    // Compute the edge's centroid, evaluate the sdf there, and add those with
    // signum zero to the boundary:
    array<num_t, 3> centroid_sd;
    array<sint_t, 3> centroid_sg;
    for (auto vidx : vertex_indices(tri)) {
      auto nvidx        = next_vx(vidx);
      auto s            = segment(vxs[vidx], vxs[nvidx]);
      auto sc           = centroid(s);
      centroid_sd[vidx] = sdf(sc);
      centroid_sg[vidx] = math::signum(centroid_sd[vidx]);
      if (centroid_sg[vidx] == 0) { result.boundary.push_back(s); }
    }
    // If they are all zero try to handle the triangle-like sd-field by using
    // the value at the triangles centroid to detect whether the triangle is
    // inside or outside.
    if (ranges::all_of(centroid_sg, 0)) {
      auto sdc = sdf(centroid(tri));
      auto sgc = math::signum(sdc);
      if (sgc == 0) {
        HM3_FATAL_ERROR("Signed-distance function is not a signed-distance "
                        "field! It is zero at all vertices of the triangle as "
                        "well as in the triangle centroid!");
      }

      if (sgc == 1) { result.inside.push_back({tri, sg}); }
      if (sgc == -1) { result.outside.push_back({tri, sg}); }
      return result;
    }
    // If more than one edge's centroid is not zero nothing can be done:
    if (ranges::count(centroid_sg, [](auto&& i) { return i != 0; }) > 1) {
      HM3_FATAL_ERROR("Probably broken signed-distance field: all vertices of "
                      "a trinagle are cut by the zero-level, but more than one "
                      "edge's centroid is not on the zero level ¯\_(ツ)_/¯");
    }
    // Otherwise detect inside/outside and add the triangle itself as shape:
    if (range::any_of(centroid_sg, 1)) {
      result.inside.push_back({tri, sg});
      return resut;
    }
    if (range::any_of(centroid_sg, -1)) {
      result.outside.push_back({tri, sg});
      return result;
    }
  }

  // If two vertices are intersected by the zero level, we assume that
  // one edge of the triangle corresponds to the boundary:
  if (no_cut_vertices == 2) {
    for (auto&& i : vertex_indices(tri)) {
      if (sg[i] == 0 and sg[next_vx(i)] == 0) {
        result.boundary.emplace_back({segment<Nd>({vxs[i], vxs[next_vx(i)]})});
        break;
      }
    }
  }

  // If all vertices inside we are done, the boundary, if any, has already been
  // added:
  if (ranges::all_of(sg, [](auto i) { return i == 1 or i == 0 })) {
    result.inside.emplace_back({tri, sg});
    return result;
  }
  // If all vertices outside we are done, the boundary, if any, has already been
  // added:
  if (ranges::all_of(sg, [](auto i) { return i == -1 or i == 0 })) {
    result.outside.emplace_back({tri, sg});
    return result;
  }

  HM3_ASSERT(no_cut_vertices == 0 or no_cut_vertices == 1, "");

  // From here till the end, we have some vertices of different signs, so the
  // intersection will have a single boundary, some part inside, and some part
  // outside:
  result.inside.push_back();
  result.outside.push_back();
  result.boundary.push_back();

  vidx_t no_cutpoints_found = 0;
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
    if (vx_sg == 0) {
      result.inside.vertices.push_back(vxs[vidx]);
      result.outside.vertices.push_back(vxs[vidx]);
      result.inside.signum.push_back(vx_sg);
      result.outside.signum.push_back(vx_sg);
      boundary_cut_points.push_back(vxs[vidx]);
      ++no_cutpoints_found;
      continue;  // we are then done since we found the cut points
    } else if (vx_sg == 1) {
      result.inside.vertices.push_back(vxs[vidx]);
      result.inside.signum.push_back(vx_sg);
    } else if (vx_sg == -1) {
      result.outside.vertices.push_back(vxs[vidx]);
      result.outside.signum.push_back(vx_sg);
    } else {
      HM3_FATAL_ERROR("unreachable");
    }

    // If:
    // - the sign of the next vertex is the same, this edge is not
    //   intersected, so we just move on to the next edge.
    // - the sign of the next vertex is zero, it will be handled in the next
    //   iteration.
    if (vx_sg == next_vx_sg || next_vx_sg == 0) { continue; }

    // Otherwise this edge is intersected by the zero level-set.
    // The point in the edge with value 0 is found by linear interpolation:
    auto x_cp = ip::linear::point_with_value(0., vxs[vidx], vxs[n_vidx], vx_sd,
                                             next_vx_sd);
    ++no_cutpoints_found;

    // Cut points are part fo both polygons and part of the surface:
    result.inside.vertices.push_back(x_cp);
    result.outside.vertices.push_back(x_cp);
    result.inside.signum.push_back(0);
    result.outside.signum.push_back(0);
    boundary_cut_points.push_back(x_cp);

    // and we move to the next edge.
  }

  // remove polygons containing only points where the level-set is zero:
  if (ranges::all_of(result.inside.signum, 0)) {
    result.inside.vertices.clear();
  }
  if (ranges::all_of(result.outside.signum, 0)) {
    result.outside.vertices.clear();
  }

  HM3_ASSERT(
   no_cutpoints_found == 1 || no_cutpoints_found == 2,
   "this cannot handle shapes intersected by multiple planes of the same"
   "signed-distance field");

  HM3_ASSERT(boundary_cut_points.size() == 1 or boundary_cut_points.size() == 2,
             "");

  if (boundary_cut_points.size() == 2) {
    result.boundary[0]
     = segment<Nd>(boundary_cut_points[0], boundary_cut_points[1]);
  }

  // Check that the result are closed polygons (not a single point or a line)
  HM3_ASSERT(result.inside.vertices.size() > 2
              || result.inside.vertices.size() == 0,
             "shape0 size: {} ", result.inside.vertices.size());
  HM3_ASSERT(result.outside.vertices.size() > 2
              || result.outside.vertices.size() == 0,
             "shape1 size: {}", result.outside.vertices.size());

  return result;
}

}  // namespace sd_detail

template <typename Shape, typename SDFunction, dim_t Nd = Shape::dimension(),
        CONCEPT_REQUIRES_(Polygon<Shape>{}
                          and SignedDistance<SDFunction, Nd>{})>
sd_detail::result<Shape> signed_distance_intersection_non_split_quad(
Shape const& s, SDFunction&& sd) noexcept {
intersection_result<Shape> cut;
using vidx_t = access::vertex_index_t<Shape>;

constexpr dim_t MaxNp = vertex_capacity(Shape{});

const auto vxs            = vertices(s);
const suint_t no_vertices = size(vxs);
auto vertex_ids           = view::iota(0_su, no_vertices);

// Compute signed distance values and signum of the vertices once:
inline_vector<num_t, MaxNp> sd_at_vertices;
inline_vector<num_t, MaxNp> sg_at_vertices;
for (auto c : vxs) {
  num_t sd_ = sd(c);
  sd_at_vertices.push_back(sd_);
  sg_at_vertices.push_back(math::signum(sd_));
}

// Index of the next vertex, wraps around for the last vertex:
auto next_vertex
 = [&](auto&& vidx) { return (vidx != no_vertices - 1) ? vidx + 1 : 0; };

vidx_t no_cutpoints_found = 0;
/// Adds vertices and cut points to each polygon
for (auto vidx : vertex_ids) {
  const auto vx_sd      = sd_at_vertices[vidx];
  const auto n_vidx     = next_vertex(vidx);
  const auto next_vx_sd = sd_at_vertices[n_vidx];
  const auto vx_sg      = math::signum(vx_sd);
  const auto next_vx_sg = math::signum(next_vx_sd);

  // If a vertex is cut by the zero level set it is part of both polygons
  // (inside and outside) and the boundary surface, otherwise it is only part
  // of
  // one polygon
  if (vx_sg == 0) {
    result.inside.vertices.push_back(vxs[vidx]);
    result.outside.vertices.push_back(vxs[vidx]);
    result.inside.signum.push_back(vx_sg);
    result.outside.signum.push_back(vx_sg);
    result.boundary[0].push_back(vxs[vidx]);
    ++no_cutpoints_found;
    continue;  // we are then done since we found the cut points
  } else if (vx_sg == 1) {
    result.inside.vertices.push_back(vxs[vidx]);
    result.inside.signum.push_back(vx_sg);
  } else if (vx_sg == -1) {
    result.outside.vertices.push_back(vxs[vidx]);
    result.outside.signum.push_back(vx_sg);
  } else {
    HM3_FATAL_ERROR("unreachable");
  }

  // If:
  // - the sign of the next vertex is the same, this edge is not
  // intersected,
  //   so we just move on to the next edge.
  // - the sign of the next vertex is zero, it will be handled in the next
  //   iteration.
  if (vx_sg == next_vx_sg || next_vx_sg == 0) { continue; }

  // Otherwise this edge is intersected by the zero level-set, so find the
  // point in the edge with value 0 by interpolating linearly:
  auto x_cp = ip::linear::point_with_value(0., vxs[vidx], vxs[n_vidx], vx_sd,
                                           next_vx_sd);
  ++no_cutpoints_found;

  // Cut points are part fo both polygons and part of the surface:
  result.inside.vertices.push_back(x_cp);
  result.outside.vertices.push_back(x_cp);
  result.inside.signum.push_back(0);
  result.outside.signum.push_back(0);
  result.boundary[0].push_back(x_cp);

  // and we move to the next edge.
}

// remove polygons containing only points where the level-set is zero:
if (ranges::all_of(cut.signum_inside, [](auto i) { return i == 0; })) {
  result.inside.vertices.clear();
}
if (ranges::all_of(cut.signum_outside, [](auto i) { return i == 0; })) {
  result.outside.vertices.clear();
}

HM3_ASSERT(
 no_cutpoints_found == 1 || no_cutpoints_found == 2,
 "this cannot handle shapes intersected by multiple planes of the same"
 "signed-distance field");

// Check that the result are closed polygons (not a single point or a line)
HM3_ASSERT(result.inside.vertices.size() > 2
            || result.inside.vertices.size() == 0,
           "shape0 size: {} ", result.inside.vertices.size());
HM3_ASSERT(result.outside.vertices.size() > 2
            || result.outside.vertices.size() == 0,
           "shape1 size: {}", result.outside.vertices.size());

HM3_ASSERT(size(cut.surface) == no_cutpoints_found,
           "{} cut points found but the cut.surface has {} points",
           no_cutpoints_found, size(cut.surface));

return cut;
}

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3
*/