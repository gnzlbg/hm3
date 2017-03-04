#pragma once
/// \file
///
/// Balanced tree coarsening algorithm
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_neighbors.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>

namespace hm3::tree {

struct balanced_coarsen_fn {
  struct restriction_fn {
    template <typename ChildrenRange>
    void operator()(node_idx, ChildrenRange&&) const noexcept {}
  };

  /// Tries to coarsen a node \p n of tree \p tree while maintaining the tree
  /// balanced
  ///
  /// Node's \p n children must be leaf nodes
  ///
  /// \param tree [in] The tree on which the algorithm operates.
  /// \param n    [in] The node to coarsen within the tree (its children will be
  ///                  removed if possible).
  /// \param r    [in] A restriction from the children to its parent. It will
  ///                  only be applied if the node will be coarsened.
  ///
  /// \note Coarsening a node
  template <typename Tree, typename Restriction = restriction_fn>
  void operator()(Tree& tree, node_idx n, Restriction&& r = Restriction{}) const
   noexcept {
    if (tree.is_leaf(n) or ranges::any_of(tree.children(n), [&](node_idx c) {
          return !tree.is_leaf(c);
        })) {
      return;
    }
    auto l = node_level(tree, n);
    for (auto&& child : tree.children(n)) {
      auto child_level = l + 1;
      for (auto&& neighbor : node_neighbors(tree, child)) {
        auto neighbor_level = node_level(tree, neighbor);
        if (neighbor_level > child_level) {
          HM3_ASSERT(neighbor_level == child_level + 1,
                     "Neighbor {} with level {} of node {} with "
                     "level {} which is "
                     "a child of node {} with level {} is not balanced",
                     neighbor, neighbor_level, child, child_level, n, l);
          return;
        }
      }
    }
    r(n, tree.children(n));
    tree.coarsen(n);
  }
};

namespace {
auto&& balanced_coarsen = static_const<balanced_coarsen_fn>::value;
}  // namespace

}  // namespace hm3::tree
