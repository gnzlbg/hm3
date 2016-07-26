#pragma once
/// \file
///
/// Grid types
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/optional_idx.hpp>

namespace hm3 {
namespace grid {
namespace hierarchical {

using tree::cpidx_t;

using tree::level_idx;
using tree_node_idx = tree::node_idx;
using tree::operator"" _n;
using tree::operator"" _l;

/// Grid index (weakly typed)
using gidx_t = dim_t;

/// Index of a grid within a tree
using grid_idx = optional_idx<gidx_t, struct grid_idx_tag>;

constexpr grid_idx operator"" _g(unsigned long long int i) {
  return grid_idx{static_cast<gidx_t>(i)};
}

/// Node index (weakly typed)
using nidx_t = idx_t;

/// Index of a node within a grid
using grid_node_idx = optional_idx<nidx_t, struct grid_node_idx_tag>;

constexpr grid_node_idx operator"" _gn(unsigned long long int i) {
  return grid_node_idx{static_cast<nidx_t>(i)};
}

}  // namespace hierarchical
}  // namespace grid

using namespace grid;

}  // namespace hm3
