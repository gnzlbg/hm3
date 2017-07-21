#pragma once
/// \file
///
/// Balanced tree refinement algorithm
#include <hm3/grid/hierarchical/tree/algorithm/node_level.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/node_neighbors.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>

namespace hm3::tree {

struct balanced_refine_fn {
  struct projection_fn {
    template <typename ChildrenRange>
    void operator()(node_idx, ChildrenRange&&) const noexcept {}
  };

  /// Refines node \p n of tree \p tree while maintaining the tree balanced
  ///
  /// \param tree [in] The tree on which the algorithm operates
  /// \param n [in] The node to refine within the tree
  /// \param p [in] A projection from the parent to its newly refined children
  ///               (useful for projecting data from the parent to its children)
  template <typename Tree, typename Projection = projection_fn>
  [[nodiscard]] auto operator()(Tree& tree, node_idx n,
                                Projection&& p = Projection{}) const noexcept {
    using r_t = decltype(tree.refine(n));
    if (!tree.is_leaf(n)) { return r_t{}; }
    auto l = node_level(tree, n);
    for (auto&& neighbor : node_neighbors(tree, n)) {
      auto neighbor_level = node_level(tree, neighbor);
      if (neighbor_level == l - 1 and tree.is_leaf(neighbor)) {
        auto neighbor_children = (*this)(tree, neighbor, p);
        if (not neighbor_children) { return r_t{}; }
      }
    }
    auto children = tree.refine(n);
    p(n, children);
    return children;
  }
};

namespace {
auto&& balanced_refine = static_const<balanced_refine_fn>::value;
}  // namespace

}  // namespace hm3::tree
