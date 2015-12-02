#pragma once
/// \file
///
/// Grid types
#include <hm3/tree/types.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {
namespace grid {

using tree::level_idx;
using tree_node_idx = tree::node_idx;
using tree::operator"" _n;
using tree::operator"" _l;

/// Index of a grid within a tree
using grid_idx
 = compact_optional<empty_scalar_value<suint_t,
                                       std::numeric_limits<sidx_t>::max()>,
                    struct grid_idx_tag>;

constexpr grid_idx operator"" _g(unsigned long long int i) {
  return grid_idx{static_cast<sidx_t>(i)};
}

/// Index of a node within a grid
using node_idx
 = compact_optional<empty_scalar_value<idx_t,
                                       std::numeric_limits<idx_t>::max()>,
                    struct node_idx_tag>;

constexpr node_idx operator"" _gn(unsigned long long int i) {
  return node_idx{static_cast<idx_t>(i)};
}

using grid_node_idx = node_idx;

}  // namespace grid
}  // namespace hm3
