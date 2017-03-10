#pragma once
/// \file
///
/// Compute node level algorithm
#include <hm3/grid/hierarchical/tree/algorithm/root_traversal.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>

namespace hm3::tree {

struct node_level_fn {
  /// Level of the node \p n within the tree \p tree
  ///
  /// \param tree [in] Tree.
  /// \param n [in] Node index.
  ///
  /// Time complexity: O(log(N))
  /// Space complexity: O(1)
  template <typename Tree>
  auto operator()(Tree const& tree, node_idx n) const noexcept -> level_idx {
    lidx_t l = 0;
    root_traversal(tree, tree.parent(n), [&](node_idx) {
      ++l;
      return true;
    });
    return level_idx{l};
  }
  /// Level of node at location \p loc
  ///
  /// \param loc [in] Node location.
  ///
  /// Time complexity: O(1)
  /// Space complexity: O(1)
  template <typename Loc, CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc&& loc) const noexcept -> level_idx {
    return loc.level();
  }

  /// Returns a Range filter for nodes of the tree \p at the level \p level
  ///
  /// \param t     [in] Tree containing the nodes
  /// \param level [in] Node at this level will remain in the range after
  ///                   filtering a range of nodes with the filter.
  ///
  /// Time complexity: O(logN) per node, applied to a range of N nodes O(NlogN)
  /// Space complexity: O(1)
  template <typename Tree>
  static auto filter(Tree const& t, level_idx level) {
    return view::filter(
     [&t, level](node_idx n) { return node_level_fn{}(t, n) == level; });
  }
};

namespace {
constexpr auto const& node_level = static_const<node_level_fn>::value;
}  // namespace

}  // namespace hm3::tree
