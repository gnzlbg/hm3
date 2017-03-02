#pragma once
/// \file
///
/// Tree types
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/optional_idx.hpp>

namespace hm3 {
namespace tree {

using geometry::ambient_dimension;

/// Node index (weakly-typed)
using nidx_t = uint_t;

/// Index of a node within a tree
using node_idx = optional_idx<nidx_t, struct tree_node_idx_tag>;

constexpr node_idx operator"" _n(unsigned long long int i) {
  return node_idx{static_cast<nidx_t>(i)};
}

/// Sibling group index (weakly-typed)
using sgidx_t = idx_t;

/// Index of a sibling group within a tree
using siblings_idx = optional_idx<sgidx_t, struct siblings_tag>;

constexpr siblings_idx operator"" _sg(unsigned long long int i) {
  return siblings_idx{static_cast<sgidx_t>(i)};
}

/// Level index (weakly-typed)
using lidx_t = suint_t;

/// Index of a level within a tree
using level_idx = optional_idx<lidx_t, struct levels_tag>;

constexpr level_idx operator"" _l(unsigned long long int i) {
  return level_idx{static_cast<lidx_t>(i)};
}

/// nd-tree
template <dim_t Nd>
struct tree;

/// Child-position index (weakly-typed)
using cpidx_t = suint_t;

/// Relative child-position coordinate (weakly-typed)
///
/// TODO: before sint_t (should match coidx_t?);
using rcpidx_t = std::make_signed_t<nidx_t>;

/// Child positions
template <dim_t Nd>
using child_pos = bounded<cpidx_t, 0, math::ipow(cpidx_t(2), cpidx_t(Nd)),
                          struct child_pos_tag>;

template <typename Tree>
using child_pos_t = typename Tree::child_pos;

/// Neighbor tags
struct same_level_tag {};
struct child_level_tag {};

/// Neighbor-position index (weakly-typed)
using npidx_t = suint_t;

/// Coordinate offset index (weakly-typed)
using coidx_t = std::make_signed_t<nidx_t>;  // signed version of node index

/// Coordinate offset
///
/// Coordinate type must be signed and can be arbitrarily long
template <dim_t Nd>
using offset_t = array<coidx_t, Nd>;

}  // namespace tree
}  // namespace hm3
