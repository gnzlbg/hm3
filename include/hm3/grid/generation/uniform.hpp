#pragma once
/// \file
///
/// Uniform grid generation
#include <hm3/grid/generation/generic.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/types.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace grid {
namespace generation {

/// Refines the grid until all the leaf nodes are at the target level
struct uniform_fn {
  template <typename TreeGrid>
  auto operator()(TreeGrid& tree, const uint_t target_level) const noexcept {
    uint_t level = 0;
    generic(
     [&]() {
       return tree.nodes() | tree.leaf()
              | tree::node_level.filter(tree, level++);
     },
     [&](auto n) { tree.refine(n); },
     [&](auto) { return (level == target_level + 1) ? false : true; },
     []() { /* sort the grid ? */ });
  }
};

namespace {
auto&& uniform = static_const<uniform_fn>::value;
}  // namespace

}  // namespace generation
}  // namespace grid
}  // namespace hm3
