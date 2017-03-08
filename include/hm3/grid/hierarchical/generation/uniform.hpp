#pragma once
/// \file
///
/// Uniform grid generation
#include <hm3/grid/hierarchical/generation/generic.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/math/core.hpp>

namespace hm3::grid::hierarchical::generation {

/// Refines the grid until all the leaf nodes are at the target level
struct uniform_fn {
  template <typename TreeGrid>
  auto operator()(TreeGrid& tree, const level_idx target_level) const noexcept {
    level_idx level{0};
    generic(
     [&]() {
       return tree.nodes() | tree.leaf()
              | tree::node_level.filter(tree, level++);
     },
     [&](auto n) { tree.refine(n); },
     [&](auto) { return (level == target_level + 1_l) ? false : true; },
     []() { /* sort the grid ? */ });
  }
};

namespace {
auto&& uniform = static_const<uniform_fn>::value;
}  // namespace

}  // namespace hm3::grid::hierarchical::generation
