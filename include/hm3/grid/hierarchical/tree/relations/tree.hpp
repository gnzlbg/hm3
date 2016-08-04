#pragma once
/// \file
///
/// Tree relations
///
/// TODO: move some of these to grid relations since they are also needed by
/// structured grids
#include <hm3/geometry/dimension.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/config/fatal_error.hpp>
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
static constexpr cpidx_t no_children(dim_t nd) noexcept {
  return math::ipow(cpidx_t{2}, cpidx_t(nd));
}

/// Number of siblings of a node
///
/// Siblings are nodes sharing the same parent.
///
/// \param nd [in] spatial dimension of the node
///
/// Formula: \f$ 2^{nd} \f$
///
static constexpr cpidx_t no_siblings(dim_t nd) noexcept {
  return no_children(nd);
}

/// Number of nodes sharing a single face within a node
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of a face
///
/// Formula: \f$ 2 ^ {m} \f$ for \f$ m >= m \f$ otherwise 0.
///
static constexpr npidx_t no_nodes_sharing_face(dim_t nd, dim_t m) noexcept {
  return (nd >= m) ? math::ipow(npidx_t{2}, npidx_t(m)) : npidx_t{0};
}

/// Number of nodes sharing a single face at a given level
///
/// \param nd [in] spatial dimension of the nodes
/// \param m [in] spatial dimension of the face
/// \param level [in] level starting from the root node (which is at level 0)
///
/// Formula: \f$ (2^{m})^{l} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_sharing_face_at_level(
 dim_t nd, dim_t m, level_idx level) noexcept {
  const auto nsf = no_nodes_sharing_face(nd, m);
  return nsf == npidx_t{0} and *level == lidx_t{0}
          ? nidx_t{0}
          : math::ipow(nidx_t{nsf}, static_cast<nidx_t>(*level));
}

/// Number of node faces
///
/// \param nd [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face
///
/// Formula: \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
///
static constexpr npidx_t no_faces(dim_t nd, dim_t m) noexcept {
  return m <= nd
          ? math::ipow(npidx_t{2}, npidx_t(nd - m))
             * math::binomial_coefficient(nd, m)
          : npidx_t{0};
}

/// Number of nodes at an uniformly refined level \p level
///
/// \param nd [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ (2^{n_d})^{\mathrm{level}} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_at_uniform_level(dim_t nd, level_idx level) {
  return math::ipow(math::ipow(nidx_t{2}, nidx_t(nd)),
                    static_cast<nidx_t>(*level));
}

/// Number of nodes in a tree with a uniformly refined level \p level
///
/// \param nd [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ \sum_{l = 0}^{level} (2^{n_d})^{\mathrm{l}} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_until_uniform_level(dim_t nd,
                                                     level_idx level) {
  nidx_t no_nodes = 0;
  for (lidx_t l = 0, e = *level; l <= e; ++l) {
    no_nodes += no_nodes_at_uniform_level(nd, l);
  }
  return no_nodes;
}

/// Normalized length of a node at level \p l (for a root node of length = 1)
static constexpr num_t node_length_at_level(level_idx l) {
  return num_t{1.} / math::ipow(nidx_t{2}, static_cast<nidx_t>(*l));
}

template <dim_t Nd> struct relative_child_positions_ {
  static constexpr array<array<rcpidx_t, 0>, 0> stencil{{}};
};

template <> struct relative_child_positions_<1> {
  static constexpr array<array<rcpidx_t, 1>, 2> stencil{{
   {{-1}}, {{1}}
   //
  }};
};

template <> struct relative_child_positions_<2> {
  static constexpr array<array<rcpidx_t, 2>, 4> stencil{{
   {{-1, -1}}, {{1, -1}}, {{-1, 1}}, {{1, 1}}
   //
  }};
};

template <> struct relative_child_positions_<3> {
  static constexpr array<array<rcpidx_t, 3>, 8> stencil{{
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
template <dim_t Nd>
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
template <dim_t Nd>
constexpr auto relative_child_position(const child_pos<Nd> p)
 -> const offset_t<Nd> {
#ifdef HM3_USE_CHILDREN_LOOKUP_TABLE
  return relative_child_position_stencil<Nd>[*p];
#else
  offset_t<Nd> r;
  for (dim_t d = 0; d < Nd; ++d) {
    r[d] = (*p / math::ipow(coidx_t{2}, coidx_t{d})) % 2 ? 1 : -1;
  }
  return r;
#endif
}

/// Centroid of child at position \p child_position for a parent with centroid
/// \p parent_centroid and length \p parent_length
template <dim_t Nd>
constexpr auto child_centroid(const child_pos<Nd> child_position,
                              geometry::point<Nd> parent_centroid,
                              num_t parent_length) -> geometry::point<Nd> {
  const auto rcp = relative_child_position<Nd>(child_position);
  const num_t l4 = parent_length / 4.;
  for (dim_t d = 0; d < Nd; ++d) { parent_centroid(d) += l4 * rcp[d]; }
  return parent_centroid;
}

/// Centroid of child at position \p child_position for a \p parent geometry
template <dim_t Nd>
constexpr auto child_centroid(const child_pos<Nd> child_position,
                              geometry::box<Nd> parent) -> geometry::point<Nd> {
  return child_centroid(child_position, geometry::centroid(parent),
                        geometry::bounding_length(parent));
}

template <dim_t Nd>
constexpr auto child_geometry(const child_pos<Nd> child_position,
                              geometry::box<Nd> parent) -> geometry::box<Nd> {
  return {child_centroid(child_position, parent),
          geometry::bounding_length(parent) / 2.};
}

///@} // Tree relations

}  // namespace tree
}  // namespace hm3
