#pragma once
/// \file
///
/// Compute normalized node length algorithm
#include <hm3/tree/algorithm/node_location.hpp>
#include <hm3/tree/concepts.hpp>
#include <hm3/tree/relations/tree.hpp>
#include <hm3/tree/types.hpp>

namespace hm3 {
namespace tree {
//

/// Find the index of a node at a given location
struct node_length_fn {
  /// Normalized length of the node at location \p loc
  ///
  /// \param loc [in] Node location
  ///
  /// Time complexity: O(1)

  template <typename Loc, CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Loc&& loc) const noexcept -> num_t {
    return node_length_at_level(loc.level());
  }

  /// Normalized length of the node \p n within the tree \p t
  ///
  /// \param t [in] Tree
  /// \param n [in] Node index
  ///
  /// Time complexity: O(log(N))
  template <typename Tree>
  auto operator()(Tree const& t, node_idx n) const noexcept -> num_t {
    return (*this)(node_location(t, n));
  }
};

namespace {
constexpr auto&& node_length = static_const<node_length_fn>::value;
}  // namespace

}  // namespace tree
}  // namespace hm3
