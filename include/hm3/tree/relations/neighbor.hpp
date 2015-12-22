#pragma once
/// \file
///
/// Tree neighbor relations
#include <array>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/tree.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/stack_vector.hpp>
/// Use look-up table for the same level neighbors instead of
/// arithmetic operations
#define HM3_USE_NEIGHBOR_LOOKUP_TABLE

namespace hm3 {
namespace tree {

/// \name Neighbor relations
///@{

/// Number of node neighbors at the node level (same level)
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face between node and neighbors
///
/// Formula: \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
///
static constexpr uint_t no_neighbors(uint_t nd, uint_t m,
                                     same_level_tag) noexcept {
  return no_faces(nd, m);
}

/// Number of node neighors at the children level of a node (node level + 1)
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face
///
/// Formula: number of nodes sharing a face * number of neighbors
///
static constexpr uint_t no_neighbors(uint_t nd, uint_t m, child_level_tag) {
  return no_nodes_sharing_face(nd, m) * no_neighbors(nd, m, same_level_tag{});
}

/// \name Stencils of neighbor children sharing a m dimensional face with a node
/// of dimension nd
///@{

template <int Nd, int M> struct neighbor_children_sharing_face_ {
  static constexpr stack::vector<stack::vector<child_pos<Nd>, 0>, 0> stencil() {
    return {};
  }
};

/// One dimensional stencil across faces
///
///  |-- Left neighbor --|-- Node --|-- Right neighbor --|
///  |         | child 1 |          | child 0  |         |
///
template <> struct neighbor_children_sharing_face_<1, 0> {
  static constexpr std::array<std::array<child_pos<1>, 1>, 2> stencil{
   {{{1}}, {{0}}}};
};

template <> struct neighbor_children_sharing_face_<2, 1> {
  static constexpr std::array<std::array<child_pos<2>, 2>, 4> stencil{
   {{{1, 3}}, {{0, 2}}, {{2, 3}}, {{0, 1}}}};
};

template <> struct neighbor_children_sharing_face_<2, 0> {
  static constexpr std::array<std::array<child_pos<2>, 1>, 4> stencil{
   {{{3}}, {{2}}, {{1}}, {{0}}}};
};

template <> struct neighbor_children_sharing_face_<3, 2> {
  static constexpr std::array<std::array<child_pos<3>, 4>, 6> stencil{
   {{{1, 3, 5, 7}},
    {{0, 2, 4, 6}},
    {{2, 3, 6, 7}},
    {{0, 1, 4, 5}},
    {{4, 5, 6, 7}},
    {{0, 1, 2, 3}}}};
};

template <> struct neighbor_children_sharing_face_<3, 1> {
  static constexpr std::array<std::array<child_pos<3>, 2>, 12> stencil{
   {{{3, 7}},
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

template <> struct neighbor_children_sharing_face_<3, 0> {
  static constexpr std::array<std::array<child_pos<3>, 1>, 8> stencil{
   {{{7}}, {{6}}, {{5}}, {{4}}, {{3}}, {{2}}, {{1}}, {{0}}}};
};

namespace {
template <int Nd, int M>
static constexpr auto neighbor_children_sharing_face
 = neighbor_children_sharing_face_<Nd, M>::stencil;
}  // namespace

///@}  // Neighbor children sharing face stencils

/// Normalized displacement from node center to node neighbor. The unit length
/// is the length of the node.
template <int Nd> using neighbor_offset = std::array<int_t, Nd>;

/// \name Neighbor lookup tables
///
/// Neighbor positions at same level:
///
/// In 1D:        across Nd-1: 2 faces
///
///         |--- 0 ---|--- * ---|--- 1 ---|   ----> d_0
///
/// In 2D:   across Nd-1: 4 faces   across Nd-2: 4 corners
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
/// In 3D: across Nd-1: 6 faces  across Nd-2: 12 edges  across Nd-3: 8
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

template <int Nd, int M> struct neighbor_lookup_table_ {
  static constexpr stack::vector<neighbor_offset<Nd>, 0> stencil{};
};

/// 1D: across faces
template <> struct neighbor_lookup_table_<1, 0> {
  static constexpr std::array<neighbor_offset<1>, 2> stencil{{
   {{-1}}, {{1}}  //
  }};
};

/// 2D: across faces
template <> struct neighbor_lookup_table_<2, 1> {
  static constexpr std::array<neighbor_offset<2>, 4> stencil{{
   {{-1, 0}}, {{1, 0}}, {{0, -1}}, {{0, 1}}  //
  }};
};

/// 2D: across edges
template <> struct neighbor_lookup_table_<2, 0> {
  static constexpr std::array<neighbor_offset<2>, 4> stencil{{
   {{-1, -1}}, {{1, -1}}, {{-1, 1}}, {{1, 1}}  //
  }};
};

/// 3D: across faces
template <> struct neighbor_lookup_table_<3, 2> {
  static constexpr std::array<neighbor_offset<3>, 6> stencil{{
   {{-1, 0, 0}},
   {{1, 0, 0}},
   {{0, -1, 0}},
   {{0, 1, 0}},
   {{0, 0, -1}},
   {{0, 0, 1}}  //
  }};
};

/// 3D: across edges
template <> struct neighbor_lookup_table_<3, 1> {
  static constexpr std::array<neighbor_offset<3>, 12> stencil{{
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
template <> struct neighbor_lookup_table_<3, 0> {
  static constexpr std::array<neighbor_offset<3>, 8> stencil{{
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
template <int Nd, int M>
constexpr auto neighbor_lookup_table = neighbor_lookup_table_<Nd, M>::stencil;
}  // namespace

///@}  // Neighbor lookup tables

template <int Nd, int M> struct manifold_neighbors;

/// Neighbor of an Nd-dimensional node across a (Nd - M)-dimensional face
// ///
/// TODO: simplify this and provide a way of constructing custom neighbor search
/// tables
template <int Nd, int M> struct manifold_neighbors : geometry::dimensional<Nd> {
  static_assert(Nd >= 0 and Nd <= 3, "");

  using geometry::dimensional<Nd>::dimension;
  using geometry::dimensional<Nd>::dimensions;
  static constexpr uint_t rank() noexcept { return M; }

  static constexpr uint_t size() noexcept {
    return no_neighbors(Nd, Nd - M, same_level_tag{});
  }

  using neighbor_idx = bounded<uint_t, 0_u, size(), manifold_neighbors<Nd, M>>;

#ifdef HM3_USE_NEIGHBOR_LOOKUP_TABLE
  static constexpr auto same_level_stencil() noexcept {
    return neighbor_lookup_table<Nd, Nd - M>;
  }
#endif
  static constexpr auto child_level_stencil() noexcept {
    return neighbor_children_sharing_face<Nd, Nd - M>;
  }

  static neighbor_idx idx(uint_t i) noexcept { return neighbor_idx(i); }

  /// Range of neighbor positions
  auto operator()() const noexcept { return neighbor_idx::rng(); }

  /// Returns the offset for the neighbor in position \p i
  constexpr neighbor_offset<Nd> operator[](neighbor_idx i) const noexcept {
    HM3_ASSERT(i, "");
    const auto n = *i;
#ifdef HM3_USE_NEIGHBOR_LOOKUP_TABLE
    return same_level_stencil()[n];
#else
    neighbor_offset<Nd> o;
    switch (m) {
      case 1: {  // Nd - 1
        for (auto&& d : dimensions()) {
          o[d] = (n / 2) == d % Nd ? (n % 2 ? 1 : -1) : 0;
        }
        return o;
      }
      case 2: {  // Nd - 2
        if (n < 4) {
          for (auto&& d : dimensions()) {
            o[d] = (n / math::ipow(2_u, d)) % 2 ? 1 : -1;
          }
          if (Nd == 3) { o[2] = 0; }
        } else {
          const uint_t c = (n / 2) % 2;
          const uint_t v = n % 2 == 0 ? -1 : 1;
          o[0]           = (!c) * v;
          o[1]           = c * v;
          o[2]           = (n - 4) / 4 == 0 ? -1 : 1;
        }
        return o;
      }
      case 3: {  // Nd - 3
        for (auto&& d : dimensions()) {
          o[d] = (n / math::ipow(2_u, d)) % 2 ? 1 : -1;
        }
        return o;
      }
      default: { HM3_FATAL_ERROR("unimplemented"); }
    }
#endif
  }

  auto offsets() const noexcept {
    return (*this)()
           | ranges::view::transform([&](auto&& idx) { return (*this)[idx]; });
  }

  static constexpr auto children_sharing_face(neighbor_idx i) noexcept {
    HM3_ASSERT(ranges::size(child_level_stencil()) != 0_u,
               "error: neighbor_idx {} | Nd {} | M {}", *i, Nd, M);
    return child_level_stencil()[*i];
  }

  static constexpr auto no_same_level_neighbors() noexcept {
    return no_neighbors(Nd, M, same_level_tag{});
  }
  static constexpr auto no_child_level_neighbors() noexcept {
    return no_neighbors(Nd, M, child_level_tag{});
  }
};

template <typename T> using neighbor_idx_t = typename T::neighbor_idx;

template <int Nd> using face_neighbors   = manifold_neighbors<Nd, 1>;
template <int Nd> using edge_neighbors   = manifold_neighbors<Nd, 2>;
template <int Nd> using corner_neighbors = manifold_neighbors<Nd, 3>;

/// neighbor of a nd-dimensional node across an m dimensional surface
template <int Nd, int M>
using surface_neighbors =
 // points:
 meta::
  if_c<M == 1, meta::if_c<Nd == 1, face_neighbors<1>,
                          meta::if_c<Nd == 2, edge_neighbors<2>,
                                     meta::if_c<Nd == 3, corner_neighbors<3>,
                                                meta::nil_>>>,
       // lines:
       meta::
        if_c<M == 2,
             meta::if_c<Nd == 2, face_neighbors<2>,
                        meta::if_c<Nd == 3, edge_neighbors<3>, meta::nil_>>,
             // planes:
             meta::if_c<M == 3 and Nd == 3, face_neighbors<3>, meta::nil_>>>;

/// Max number of neighbors at children level across all manifolds
constexpr auto max_no_neighbors(int nd) {
  int s = 0;
  for (int i = 1; i != nd + 1; ++i) {
    s += no_neighbors(nd, i, child_level_tag{});
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
      const constexpr std::array<uint_t, 12> stencil{
       {3, 2, 1, 0, 9, 8, 11, 10, 5, 4, 7, 6}};
      return stencil[*p];
    }
    case 3: {
      return no_children(manifold::dimension()) - 1 - (*p);
    }
    default: { HM3_FATAL_ERROR("unimplemented"); }
  }
}

template <uint_t Nd, typename F> void for_each_neighbor_manifold(F&& f) {
  using manifold_rng = meta::as_list<meta::integer_range<int, 1, Nd + 1>>;
  meta::for_each(manifold_rng{},
                 [&](auto m) {
                   using manifold = manifold_neighbors<Nd, decltype(m){}>;
                   f(manifold{});
                 });
}

///@} Neighbor relations

}  // namespace tree
}  // namespace hm3
