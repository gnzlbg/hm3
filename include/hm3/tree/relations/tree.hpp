#pragma once
/// \file
///
/// Tree relations
#include <array>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/types.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/fatal_error.hpp>
#include <hm3/utility/math.hpp>
/// Use look-up table for the relative children position instead of arithmetic
/// operations
#define HM3_USE_CHILDREN_LOOKUP_TABLE

namespace hm3 {
namespace tree {
//

/// \name Tree relations
///@{

/// Number of children of a node
///
/// \param nd [in] spatial dimension of the node
///
/// Formula: \f$ 2^{nd} \f$
///
static constexpr uint_t no_children(uint_t nd) noexcept {
  return math::ipow(2_u, nd);
}

/// Number of siblings of a node
///
/// Siblings are nodes sharing the same parent.
///
/// \param nd [in] spatial dimension of the node
///
/// Formula: \f$ 2^{nd} \f$
///
static constexpr uint_t no_siblings(uint_t nd) noexcept {
  return no_children(nd);
}

/// Number of nodes sharing a single face within a node
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of a face
///
/// Formula: \f$ 2 ^ {m} \f$ for \f$ m >= m \f$ otherwise 0.
///
static constexpr uint_t no_nodes_sharing_face(uint_t nd, uint_t m) noexcept {
  return (nd >= m) ? math::ipow(2_u, m) : 0_u;
}

/// Number of nodes sharing a single face at a given level
///
/// \param nd [in] spatial dimension of the nodes
/// \param m [in] spatial dimension of the face
/// \param level [in] level starting from the root node (which is at level 0)
///
/// Formula: \f$ (2^{m})^{l} \f$
///
static constexpr uint_t no_nodes_sharing_face_at_level(
 uint_t nd, uint_t m, level_idx level) noexcept {
  const auto nsf = no_nodes_sharing_face(nd, m);
  return nsf == 0_u and *level == 0_u
          ? 0_u
          : math::ipow(nsf, static_cast<uint_t>(*level));
}

/// Number of node faces
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face
///
/// Formula: \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
///
static constexpr uint_t no_faces(uint_t nd, uint_t m) noexcept {
  return m <= nd ? math::ipow(2_u, nd - m) * math::binomial_coefficient(nd, m)
                 : 0_u;
}

/// Number of nodes at an uniformly refined level \p level
///
/// \param nd [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ (2^{n_d})^{\mathrm{level}} \f$
///
static constexpr uint_t no_nodes_at_uniform_level(uint_t nd, level_idx level) {
  return math::ipow(math::ipow(2_u, nd), static_cast<uint_t>(*level));
}

/// Number of nodes in a tree with a uniformly refined level \p level
///
/// \param nd [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ \sum_{l = 0}^{level} (2^{n_d})^{\mathrm{l}} \f$
///
static constexpr uint_t no_nodes_until_uniform_level(uint_t nd,
                                                     level_idx level) {
  uint_t no_nodes = 0;
  for (suint_t l = 0, e = *level; l <= e; ++l) {
    no_nodes += no_nodes_at_uniform_level(nd, l);
  }
  return no_nodes;
}

/// Normalized length of a node at level \p l (for a root node of length = 1)
static constexpr num_t node_length_at_level(const level_idx l) {
  return num_t{1} / math::ipow(2_u, static_cast<uint_t>(*l));
}

template <int Nd> struct relative_child_positions_ {
  static constexpr std::array<std::array<int_t, 0>, 0> stencil{{}};
};

template <> struct relative_child_positions_<1> {
  static constexpr std::array<std::array<int_t, 1>, 2> stencil{{
   {{-1}}, {{1}}
   //
  }};
};

template <> struct relative_child_positions_<2> {
  static constexpr std::array<std::array<int_t, 2>, 4> stencil{{
   {{-1, -1}}, {{1, -1}}, {{-1, 1}}, {{1, 1}}
   //
  }};
};

template <> struct relative_child_positions_<3> {
  static constexpr std::array<std::array<int_t, 3>, 8> stencil{{
   {{-1, -1, -1}},
   {{1, -1, -1}},
   {{-1, 1, -1}},
   {{1, 1, -1}},
   {{-1, -1, 1}},
   {{1, -1, 1}},
   {{-1, 1, 1}},
   {{1, 1, 1}}
   //
  }};
};

namespace {
template <int Nd>
static constexpr auto relative_child_position_stencil
 = relative_child_positions_<Nd>::stencil;
}  // namespace

/// Relative position of the children w.r.t. their parent's center:
///
/// \tparam Nd number of spatial dimensions of the node
/// \param [in] p position of the children
///
/// \returns relative position (+1/-1, ...) of child \p w.r.t. his parent node
/// center (an array of size nd)
///
/// That is:
///              __________________________
///            /|   pos: 6   |   pos: 7  /|
///           / | (-1,+1,+1) | (+1,+1,+1) |
///          /  |____________|____________|
///         /   |   pos: 4   |   pos: 5   |
///        /    | (-1,-1,+1) | (+1,-1,+1) |
///       /     |____________|____________|
///      /     /                   /     /
///     /_____/___________________/     /
///    |   pos: 2   |   pos: 3   |     /    d (1) ^
///    | (-1,+1,-1) | (+1,+1,-1) |    /           |     ^ z (2)
///    |____________|____________|   /            |    /
///    |   pos: 0   |   pos: 1   |  /             |  /
///    | (-1,-1,-1) | (+1,-1,-1) | /              |/
///    |____________|____________|/               o-------> x (0)
///
///
///
///
///
template <int Nd>
static constexpr auto relative_child_position(const child_pos<Nd> p)
 -> const std::array<int_t, Nd> {
#ifdef HM3_USE_CHILDREN_LOOKUP_TABLE
  return relative_child_position_stencil<Nd>[*p];
#else
  std::array<int_t, Nd> r;
  for (auto&& d : dimensions(Nd)) {
    r[d] = (*p / math::ipow(2_u, d)) % 2 ? 1 : -1;
  }
  return r;
#endif
}

///@} // Tree relations

}  // namespace tree
}  // namespace hm3
