#pragma once
/// \file
///
/// Node-to-root traversal algorithm
#include <hm3/tree/types.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace tree {

struct root_traversal_fn {
  /// Executes function \p f (node) -> bool on each node between node
  /// \p n and the root node or until f(node) returns false.
  ///
  /// \param tree [in] Tree on which the node \p n is located
  /// \param n [in] Start node
  /// \param f [in] Function f(node) -> bool to apply to all nodes between \p n
  ///               and the root node
  ///
  /// \returns Returns the last node to which f was applied to.
  ///
  /// Time complexity: O(log(N))
  /// Space complexity: O(1)
  ///
  template <typename Tree, typename F>
  auto operator()(Tree const& tree, node_idx n, F&& f) const noexcept
   -> node_idx {
    while (n and f(n)) { n = tree.parent(n); }
    return n;
  }
};

namespace {
constexpr auto&& root_traversal = static_const<root_traversal_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
