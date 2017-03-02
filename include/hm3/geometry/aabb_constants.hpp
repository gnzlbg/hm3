#pragma once
/// \file
///
/// AABB constants.
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/fatal_error.hpp>

namespace hm3::geometry {

namespace aabb_constants {

/// Number of vertices in an \p nd dimensional AABB.
constexpr dim_t vertex_size(dim_t nd) noexcept {
  return math::ipow(dim_t{2}, nd);
}

namespace detail {
constexpr dim_t face_size(dim_t nd, dim_t m) noexcept {
  return m <= nd ? math::ipow(dim_t{2}, dim_t(nd - m))
                    * math::binomial_coefficient(nd, m)
                 : dim_t{0};
}

}  // namespace detail

/// Number of m-dimensional edges in an \p nd dimensional AABB.
constexpr dim_t face_size(dim_t nd, dim_t m) noexcept {
  static_assert(detail::face_size(1, 3) == 0);
  static_assert(detail::face_size(1, 2) == 0);
  static_assert(detail::face_size(1, 1) == 1);
  static_assert(detail::face_size(1, 0) == 2);

  static_assert(detail::face_size(2, 3) == 0);
  static_assert(detail::face_size(2, 2) == 1);
  static_assert(detail::face_size(2, 1) == 4);
  static_assert(detail::face_size(2, 0) == 4);

  static_assert(detail::face_size(3, 3) == 1);
  static_assert(detail::face_size(3, 2) == 6);
  static_assert(detail::face_size(3, 1) == 12);
  static_assert(detail::face_size(3, 0) == 8);
  return detail::face_size(nd, m);
}

/// Relative position of AABB vertices (+-1) wrt to the AABB centroid. This
/// determines the order of the AABB vertices.
///
/// Order: Counter Clow Wise (CCW):
///
///       7         6
///        *-------*
///       /|      /|
///      / |     / |       x(1)   x(2)
///   3 *-------* 2|        ^   ^
///     |  *----|--*        |  /
///     | / 4   | / 5       | /
///     |/      |/          |/
///     *-------*           *------> x(0)
///     0       1
///
/// Project down for 2D (0, 1, 2, 3) and 1D (0, 1) vertices.
///
constexpr sint_t relative_vertex_position(dim_t vertex_pos,
                                          dim_t dimension) noexcept {
  /// TODO: table lookup
  switch (dimension) {
    case 0: {
      return ((vertex_pos + 1) / 2 % 2) != 0 ? 1 : -1;
    }
    case 1: {
      return ((vertex_pos / 2) % 2) != 0 ? 1 : -1;
    }
    case 2: {
      return ((vertex_pos / 4) % 4) != 0 ? +1 : -1;
    }
    default: { HM3_FATAL_ERROR("unknown dimension {}", dimension); }
  }
}

template <dim_t Ad, dim_t... Is>
constexpr point<Ad> relative_vertex_position(
 suint_t vertex_pos, std::integer_sequence<dim_t, Is...>) noexcept {
  return point<Ad>{
   static_cast<num_t>(relative_vertex_position(vertex_pos, Is))...};
}

template <dim_t Ad>
constexpr point<Ad> relative_vertex_position(dim_t vertex_pos) noexcept {
  return relative_vertex_position<Ad>(vertex_pos,
                                      std::make_integer_sequence<dim_t, Ad>{});
}

constexpr pair<dim_t, dim_t> edge_vertices(dim_t edge_idx,
                                           dim_t ambient_dimensions) noexcept {
  HM3_ASSERT(edge_idx < face_size(ambient_dimensions, 1), "");
  switch (ambient_dimensions) {
    case 1: {
      return make_pair(dim_t{0}, dim_t{1});
    }
    case 2: {
      dim_t first[4] = {0, 1, 2, 3};
      dim_t last[4]  = {1, 2, 3, 0};
      return make_pair(first[edge_idx], last[edge_idx]);
    }
    case 3: {
      dim_t first[12] = {0, 1, 3, 2, 0, 1, 0, 3, 4, 5, 4, 7};
      dim_t last[12]  = {4, 5, 7, 6, 3, 2, 1, 2, 7, 6, 5, 6};
      return make_pair(first[edge_idx], last[edge_idx]);
    }
    default: { HM3_FATAL_ERROR("unknown dimension {}", ambient_dimensions); }
  }
}

constexpr dim_t face_vertices(dim_t fidx, dim_t vidx) noexcept {
  HM3_ASSERT(fidx < 6, "fidx {} out-of-bounds [0, 6)", fidx);
  HM3_ASSERT(vidx < 4, "vidx {} out-of-bounds [0, 4)", vidx);
  constexpr dim_t face_vertices_[6][4] = {
   //
   {0, 4, 7, 3},  // face 0 -x
   {1, 5, 6, 2},  // face 1 +x
   {0, 1, 5, 4},  // face 2 -y
   {3, 2, 6, 7},  // face 3 +y
   {3, 2, 1, 0},  // face 4 -z
   {7, 6, 5, 4}   // face 5 +z
  };

  return face_vertices_[fidx][vidx];
}

}  // namespace aabb_constants

}  // namespace hm3::geometry

/* relative face position
#pragma once
/// \file
///
/// AABB relative face position (from segment, polygon, polyhedron faces to
/// tree::neighbor faces).
#include <hm3/geometry/access/face.hpp>
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/config/fatal_error.hpp>

namespace hm3::geometry::aabb_primitive {

template <dim_t Nd>
struct face_idx_to_face_neighbor {
  using face_neighbor_ids    = tree::face_neighbors<Nd>;
  using face_neighbor_offset = typename face_neighbor_ids::offset_t;
  using face_neighbor_index  = typename face_neighbor_ids::neighbor_idx;

  CONCEPT_REQUIRES(Nd == 1)
  static constexpr face_neighbor_index neighbor_idx(suint_t face_idx) noexcept {
    return face_neighbor_ids{}(face_idx);
  }

  CONCEPT_REQUIRES(Nd == 2)
  static constexpr face_neighbor_index neighbor_idx(suint_t face_idx) noexcept {
    constexpr array<suint_t, 4> neighbor_ids{{2, 1, 3, 0}};
    return face_neighbor_ids{}(neighbor_ids[face_idx]);
  }

  // CONCEPT_REQUIRES(Nd == 3)
  // static constexpr face_neighbor_index neighbor_idx(fidx_t face_idx) noexcept
  // {
  //   constexpr array<fidx_t, 6> neighbor_ids{{2, 1, 3, 0}};
  //   return face_neighbor_ids{}(neighbor_ids[face_idx]);
  // }

  static constexpr face_neighbor_offset neighbor_offset(
   suint_t face_idx) noexcept {
    return face_neighbor_ids{}(neighbor_idx(face_idx));
  }
};

}  // namespace hm3::geometry::aabb_primitive
*/
