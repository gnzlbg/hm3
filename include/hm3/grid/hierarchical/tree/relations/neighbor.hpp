#pragma once
/// \file
///
/// Tree neighbor relations
///
/// TODO: move some of this to e.g. grid/relations since these are also used by
/// structured grids
#include <hm3/geometry/fwd.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/hierarchical/tree/tree.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/math/core.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/fixed_capacity_vector.hpp>
/// Use look-up table for the same level neighbors instead of
/// arithmetic operations
#define HM3_USE_NEIGHBOR_LOOKUP_TABLE

namespace hm3::tree {

/// \name Neighbor relations
///@{

/// Number of node neighbors at the node level (same level)
///
/// \param ad [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face between node and neighbors
///
/// Formula: \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
///
/// Returns 0 if !(ad > m).
static constexpr npidx_t no_neighbors(dim_t ad, dim_t m,
                                      same_level_tag) noexcept {
  return ad >= m ? no_faces(ad, m) : 0;
}

/// Number of node neighors at the children level of a node (node level + 1)
///
/// \param ad [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face
///
/// Formula: number of nodes sharing a face * number of neighbors
///
/// Returns 0 if !(ad > m).
static constexpr npidx_t no_neighbors(dim_t ad, dim_t m, child_level_tag) {
  return ad >= m
          ? no_nodes_sharing_face(ad, m) * no_neighbors(ad, m, same_level_tag{})
          : 0;
}

/// \name Stencils of neighbor children sharing a m dimensional face with a node
/// of dimension ad
///@{

template <dim_t Ad, dim_t M>
struct neighbor_children_sharing_face_ {
  static constexpr fixed_capacity_vector<
   fixed_capacity_vector<child_pos<Ad>, 0>, 0>
   stencil() {
    return {};
  }
};

/// One dimensional stencil across faces
///
///  |-- Left neighbor --|-- Node --|-- Right neighbor --|
///  |         | child 1 |          | child 0  |         |
///
template <>
struct neighbor_children_sharing_face_<1, 0> {
  static constexpr array<array<child_pos<1>, 1>, 2> stencil{{{{1}}, {{0}}}};
};

template <>
struct neighbor_children_sharing_face_<2, 1> {
  static constexpr array<array<child_pos<2>, 2>, 4> stencil{
   {{{1, 3}}, {{0, 2}}, {{2, 3}}, {{0, 1}}}};
};

template <>
struct neighbor_children_sharing_face_<2, 0> {
  static constexpr array<array<child_pos<2>, 1>, 4> stencil{
   {{{3}}, {{2}}, {{1}}, {{0}}}};
};

template <>
struct neighbor_children_sharing_face_<3, 2> {
  static constexpr array<array<child_pos<3>, 4>, 6> stencil{{{{1, 3, 5, 7}},
                                                             {{0, 2, 4, 6}},
                                                             {{2, 3, 6, 7}},
                                                             {{0, 1, 4, 5}},
                                                             {{4, 5, 6, 7}},
                                                             {{0, 1, 2, 3}}}};
};

template <>
struct neighbor_children_sharing_face_<3, 1> {
  static constexpr array<array<child_pos<3>, 2>, 12> stencil{{{{3, 7}},
                                                              {{2, 6}},
                                                              {{1, 5}},
                                                              {{0, 4}},
                                                              {{5, 7}},
                                                              {{4, 6}},
                                                              {{6, 7}},
                                                              {{4, 5}},
                                                              {{1, 3}},
                                                              {{0, 2}},
                                                              {{2, 3}},
                                                              {{0, 1}}}};
};

template <>
struct neighbor_children_sharing_face_<3, 0> {
  static constexpr array<array<child_pos<3>, 1>, 8> stencil{
   {{{7}}, {{6}}, {{5}}, {{4}}, {{3}}, {{2}}, {{1}}, {{0}}}};
};

namespace {
template <dim_t Ad, dim_t M>
static constexpr auto neighbor_children_sharing_face
 = neighbor_children_sharing_face_<Ad, M>::stencil;
}  // namespace

///@}  // Neighbor children sharing face stencils

/// Normalized displacement from node center to node neighbor. The unit length
/// is the length of the node.
template <dim_t Ad>
using neighbor_offset = offset_t<Ad>;

/// \name Neighbor lookup tables
///
/// Neighbor positions at same level:
///
/// In 1D:        across Ad-1: 2 faces
///
///         |--- 0 ---|--- * ---|--- 1 ---|   ----> d_0
///
/// In 2D:   across Ad-1: 4 faces   across Ad-2: 4 corners
///                ______           ______      ______
///               |     |          |     |     |     |
///               |  3  |          |  2  |     |  3  |    d_1 ^
///          _____|_____|______    |_____|_____|_____|        |
///         |     |     |     |          |     |              |
///         |  0  |  *  |  1  |          |  *  |              *----> d_0
///         |_____|_____|_____|    ______|_____|______
///               |     |          |     |     |     |
///               |  2  |          |  0  |     |  1  |
///               |_____|          |_____|     |_____|
///
/// In 3D: across Ad-1: 6 faces  across Ad-2: 12 edges  across Ad-3: 8
/// corners
///
///         3    5          ___11___         6______7   d_1 ^   ^ d_3
///         |  /        2  /|   3 /|        /|     /|       |  /
///         |/           /__7___/  |      2______3  |       |/
///  0 -----*----- 1    |  8|  |   |9    |   |  |   |       *--->d_0
///       / |           |   |__|10_|     |   4__|___5
///     /   |          4|  /0  |5 /1     |  /   |  /
///   4     2           |/_____|/        |0_____1/
///                         6
///
/// In the following stencils, for a given neighbor position an offset vector is
/// returned. The offset is is a normalized displacement from the node center to
/// the node neighbor. The length 1 represents one cell length at the nodes
/// level.
///
///@{

template <dim_t Ad, dim_t M>
struct neighbor_lookup_table_ {
  static constexpr fixed_capacity_vector<neighbor_offset<Ad>, 0> stencil{};
};

/// 1D: across faces
template <>
struct neighbor_lookup_table_<1, 0> {
  static constexpr array<neighbor_offset<1>, 2> stencil{{
   {{-1}}, {{1}}  //
  }};
};

/// 2D: across faces
template <>
struct neighbor_lookup_table_<2, 1> {
  static constexpr array<neighbor_offset<2>, 4> stencil{{
   {{-1, 0}}, {{1, 0}}, {{0, -1}}, {{0, 1}}  //
  }};
};

/// 2D: across edges
template <>
struct neighbor_lookup_table_<2, 0> {
  static constexpr array<neighbor_offset<2>, 4> stencil{{
   {{-1, -1}}, {{1, -1}}, {{-1, 1}}, {{1, 1}}  //
  }};
};

/// 3D: across faces
template <>
struct neighbor_lookup_table_<3, 2> {
  static constexpr array<neighbor_offset<3>, 6> stencil{{
   {{-1, 0, 0}},
   {{1, 0, 0}},
   {{0, -1, 0}},
   {{0, 1, 0}},
   {{0, 0, -1}},
   {{0, 0, 1}}  //
  }};
};

/// 3D: across edges
template <>
struct neighbor_lookup_table_<3, 1> {
  static constexpr array<neighbor_offset<3>, 12> stencil{{
   {{-1, -1, 0}},
   {{1, -1, 0}},
   {{-1, 1, 0}},
   {{1, 1, 0}},
   {{-1, 0, -1}},
   {{1, 0, -1}},
   {{0, -1, -1}},
   {{0, 1, -1}},
   {{-1, 0, 1}},
   {{1, 0, 1}},
   {{0, -1, 1}},
   {{0, 1, 1}}
   //
  }};
};

/// 3D: across corners
template <>
struct neighbor_lookup_table_<3, 0> {
  static constexpr array<neighbor_offset<3>, 8> stencil{{
   {{-1, -1, -1}},
   {{1, -1, -1}},
   {{-1, 1, -1}},
   {{1, 1, -1}},
   {{-1, -1, 1}},
   {{1, -1, 1}},
   {{-1, 1, 1}},
   {{1, 1, 1}}  //
  }};
};

namespace {
template <dim_t Ad, dim_t M>
constexpr auto neighbor_lookup_table = neighbor_lookup_table_<Ad, M>::stencil;
}  // namespace

///@}  // Neighbor lookup tables

template <dim_t Ad, dim_t M>
struct manifold_neighbors;

/// Neighbor of an Ad-dimensional node across a (Ad - M)-dimensional face
///
/// TODO: simplify this and provide a way of constructing custom neighbor search
/// tables
template <dim_t Ad, dim_t M>
struct manifold_neighbors : geometry::with_ambient_dimension<Ad> {
  static_assert(Ad >= 0 and Ad <= 3, "");

  using geometry::with_ambient_dimension<Ad>::ambient_dimension;
  using geometry::with_ambient_dimension<Ad>::ambient_dimensions;

  using offset_t = neighbor_offset<Ad>;

  /// Rank of the neighbor manifold
  static constexpr dim_t rank() noexcept { return M; }
  /// Spatial dimension of the faces between the neighbors
  static constexpr dim_t face_dimension() noexcept {
    return Ad >= M ? Ad - M : math::highest<dim_t>;
  }
  /// Number of neighbors at the same grid level
  static constexpr npidx_t size() noexcept {
    return Ad >= M ? no_neighbors(Ad, face_dimension(), same_level_tag{}) : 0;
  }
  /// Index of the neighbors
  ///
  /// Used for iterating over the neighbors of a manifold.
  /// The manifold is the tag type of the index, so from a given index one can
  /// obtain the manifold and e.g. the offset coordinates.
  using neighbor_idx
   = bounded<npidx_t, npidx_t{0}, size(), manifold_neighbors<Ad, M>>;

  /// Index of neighbor at position \p i
  static neighbor_idx idx(npidx_t i) noexcept { return neighbor_idx(i); }

  /// Range of neighbor positions
  auto operator()() const noexcept { return neighbor_idx::rng(); }

  // static constexpr auto child_level_stencil() noexcept {
  //   return neighbor_children_sharing_face<Ad, face_dimension()>;
  // }

  /// Returns the offset for the neighbor at position \p i
  constexpr offset_t operator[](neighbor_idx i) const noexcept {
    HM3_ASSERT(i, "");
    const auto n = *i;
#ifdef HM3_USE_NEIGHBOR_LOOKUP_TABLE
    return neighbor_lookup_table<Ad, face_dimension()>[n];
#else
    offset_t o;
    switch (m) {
      case 1: {  // Ad - 1
        for (auto&& d : ambient_dimensions()) {
          o[d] = (n / 2) == d % Ad ? (n % 2 ? 1 : -1) : 0;
        }
        return o;
      }
      case 2: {  // Ad - 2
        if (n < 4) {
          for (auto&& d : ambient_dimensions()) {
            o[d] = (n / math::ipow(2_u, d)) % 2 ? 1 : -1;
          }
          if (Ad == 3) { o[2] = 0; }
        } else {
          const npidx_t c = (n / 2) % 2;
          const sint_t v  = n % 2 == 0 ? -1 : 1;
          o[0]            = (!c) * v;
          o[1]            = c * v;
          o[2]            = (n - 4) / 4 == 0 ? -1 : 1;
        }
        return o;
      }
      case 3: {  // Ad - 3
        for (auto&& d : ambient_dimensions()) {
          o[d] = (n / math::ipow(npidx_t{2}, npidx_t{d})) % 2 ? 1 : -1;
        }
        return o;
      }
      default: { HM3_FATAL_ERROR("unimplemented"); }
    }
#endif
  }

  /// Range of offsets for each neighbor
  auto offsets() const noexcept {
    return (*this)()
           | ranges::view::transform([&](auto&& idx) { return (*this)[idx]; });
  }

  /// Range of children sharing a face with neighbor at position \p i
  static constexpr auto children_sharing_face(neighbor_idx i) noexcept {
    HM3_ASSERT(
     ranges::size(neighbor_children_sharing_face<Ad, face_dimension()>) != 0_u,
     "error: neighbor_idx {} | Ad {} | M {}", *i, Ad, M);
    return neighbor_children_sharing_face<Ad, face_dimension()>[*i];
  }

  /// Number of same level neighbors
  static constexpr auto no_same_level_neighbors() noexcept {
    return no_neighbors(Ad, M, same_level_tag{});
  }
  /// Number of neighbors at the children level
  static constexpr auto no_child_level_neighbors() noexcept {
    return no_neighbors(Ad, M, child_level_tag{});
  }
};

template <typename T>
using neighbor_idx_t = typename T::neighbor_idx;

template <dim_t Ad>
using face_neighbors = manifold_neighbors<Ad, 1>;
template <dim_t Ad>
using edge_neighbors = manifold_neighbors<Ad, 2>;
template <dim_t Ad>
using corner_neighbors = manifold_neighbors<Ad, 3>;

/// neighbor of a ad-dimensional node across an m dimensional surface
template <dim_t Ad, dim_t M>
using surface_neighbors =
 // points:
 meta::if_c<
  M == 1,
  meta::if_c<Ad == 1, face_neighbors<1>,
             meta::if_c<Ad == 2, edge_neighbors<2>,
                        meta::if_c<Ad == 3, corner_neighbors<3>, meta::nil_>>>,
  // lines:
  meta::if_c<M == 2,
             meta::if_c<Ad == 2, face_neighbors<2>,
                        meta::if_c<Ad == 3, edge_neighbors<3>, meta::nil_>>,
             // planes:
             meta::if_c<M == 3 and Ad == 3, face_neighbors<3>, meta::nil_>>>;

/// Max number of neighbors at children level across all manifolds
constexpr auto max_no_neighbors(dim_t ad) {
  npidx_t s = 0;
  for (npidx_t i = 1; i != ad + 1; ++i) {
    s += no_neighbors(ad, i, child_level_tag{});
  }
  return s;
}

/// Opposite neighbor position
///
/// The neighbor with position \p p of a given node, has that node as neighbor
/// in the "opposite neighbor position" within the same neighbor set / manifold.
///
template <typename NeighborIdx>
constexpr auto opposite(NeighborIdx p) -> NeighborIdx {
  using manifold = get_tag_t<NeighborIdx>;
  switch (manifold::rank()) {
    case 1: {
      const bool f = *p % 2;
      HM3_ASSERT(f && *p > 0 || !f, "");
      auto tmp = f ? (*p - 1) : 0;
      return (!f) * (*p + 1) + f * tmp;
    }
    case 2: {
      const constexpr array<npidx_t, 12> stencil{
       {3, 2, 1, 0, 9, 8, 11, 10, 5, 4, 7, 6}};
      return stencil[*p];
    }
    case 3: {
      return no_children(manifold::ambient_dimension()) - 1 - (*p);
    }
    default: { HM3_FATAL_ERROR("unimplemented"); }
  }
}

/// Calls the unary function F with each neighbor manifold
template <dim_t Ad, typename F>
void for_each_neighbor_manifold(F&& f) {
  using manifold_rng = meta::as_list<meta::integer_range<dim_t, 1, Ad + 1>>;
  meta::for_each(manifold_rng{}, [&](auto m) {
    using manifold = manifold_neighbors<Ad, decltype(m){}>;
    f(manifold{});
  });
}

///@} Neighbor relations

}  // namespace hm3::tree
