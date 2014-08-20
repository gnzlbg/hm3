#pragma once
/// \file types.hpp
#include <hm3/types.hpp>
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace tree {

/// Index of a node within a tree
using node_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct node_idx_tag>;

constexpr node_idx operator"" _n(unsigned long long int i) {
  return node_idx{static_cast<idx_t>(i)};
}

/// Index of a sibling group within a tree
using siblings_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct siblings_tag>;

constexpr siblings_idx operator"" _sg(unsigned long long int i) {
  return siblings_idx{static_cast<idx_t>(i)};
}

/// Index of a level within a tree
using level_idx
 = compact_optional<empty_scalar_value<suint_t,
                                       std::numeric_limits<suint_t>::max()>,
                    struct levels_tag>;

constexpr level_idx operator"" _l(unsigned long long int i) {
  return level_idx{static_cast<suint_t>(i)};
}

/// nd-tree
template <uint_t nd> struct tree;

/// Child positions
template <uint_t nd>
using child_pos
 = bounded<suint_t, 0, math::ipow(2_u, nd), struct child_pos_tag>;

template <typename Tree> using child_pos_t = typename Tree::child_pos;

/// Neighbor tags
struct same_level_tag {};
struct child_level_tag {};

}  // namespace tree
}  // namespace hm3
