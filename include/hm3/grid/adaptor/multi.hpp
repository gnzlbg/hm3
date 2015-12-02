#pragma once
/// \file
///
/// Adapts a grid to store multiple solver grids inside
#include <hm3/grid/types.hpp>
#include <hm3/tree/algorithm/balanced_coarsen.hpp>
#include <hm3/tree/algorithm/balanced_refine.hpp>
#include <hm3/tree/algorithm/dfs_sort.hpp>
#include <hm3/tree/algorithm/node_location.hpp>
#include <hm3/tree/algorithm/node_neighbors.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace grid {

namespace adaptor {

/// Stores multiple grids inside a tree-like grid
template <typename TreeGrid>  //
struct multi : TreeGrid {
  /// Multi grid indices
  using data_t = dense::matrix<grid_node_idx, dense::dynamic, dense::dynamic,
                               tree_node_idx, grid_idx, dense::col_major_t>;
  data_t grids_;

  using TreeGrid::assert_node_in_use;
  using TreeGrid::nodes;
  using TreeGrid::siblings;
  using TreeGrid::children;
  using TreeGrid::neighbors;
  using TreeGrid::node;

  multi() : grids_(0, 0) {}
  multi(multi const&) = default;
  multi(multi&&)      = default;
  multi& operator=(multi const&) = default;
  multi& operator=(multi&&) = default;

  template <typename... Args>
  explicit multi(tree_node_idx node_capacity, grid_idx grid_capacity,
                 Args&&... args)
   : TreeGrid(node_capacity, std::forward<Args>(args)...)
   , grids_(*node_capacity, *grid_capacity) {}
  explicit multi(grid_idx grid_capacity, TreeGrid tree_grid)
   : TreeGrid(std::move(tree_grid))
   , grids_(*TreeGrid::capacity(), *grid_capacity) {}

  /// Number of grids
  inline grid_idx no_grids() const noexcept {
    return grid_idx{grids_().cols()};
  }

  /// Range of all grid ids.
  inline auto grids() const noexcept {
    return boxed_ints<grid_idx>(0_g, no_grids());
  }

  /// How to swap the connectivity between two nodes \p i and \p j
  inline auto data_swap() noexcept {
    return [this](tree_node_idx i, tree_node_idx j) {
      this->grids_.row(i).swap(this->grids_.row(j));
    };
  }

  template <typename At>
  void assert_grid_in_bounds(grid_idx g, At&& at) const noexcept {
    HM3_ASSERT_AT(g && g < no_grids(), "grid {} out-of-bounds [0, {})", at, g,
                  no_grids());
  }

  /// Sorts the grid using dfs
  void sort() noexcept { tree::dfs_sort(*this, data_swap()); }

  /// Refines node \p n and return the tree_node_idx of its children
  ///
  /// If p has children, this just return the children.
  /// Otherwise, it refines the node within the tree.
  auto refine(tree_node_idx n) noexcept {
    assert_node_in_use(n, HM3_AT_);
    if (TreeGrid::is_leaf(n)) {
      auto c = tree::balanced_refine(static_cast<TreeGrid&>(*this), n);
      HM3_ASSERT(c, "balanced_refine of node {} failed (size: {}, capacity: "
                    "{}, is full: {})!",
                 n, TreeGrid::size(), TreeGrid::capacity(),
                 TreeGrid::size() == TreeGrid::capacity());
      return TreeGrid::nodes(c);
    } else {
      return TreeGrid::children(n);
    }
  }

  /// Remove grid node of grid \p g at node \p n
  ///
  /// If the siblings of node \p n are leafs and contain no grid nodes from any
  /// grid they will be removed from the tree (their parent will be coarsen).
  auto remove(tree_node_idx n, grid_idx g) noexcept {
    assert_node_in_use(n, HM3_AT_);
    assert_grid_in_bounds(g, HM3_AT_);
    HM3_ASSERT(node(n, g), "node(node: {}, grid: {}) is already invalid", n, g);
    node(n, g) = grid_node_idx{};
    auto p = TreeGrid::parent(n);
    if (TreeGrid::is_leaf(n) and !TreeGrid::is_root(n)) {
      if (all_of(TreeGrid::siblings(n), [&](tree_node_idx m) {
            return all_of(grid_nodes(m), [&](grid_node_idx i) { return !i; });
          })) {
        // TreeGrid::coarsen(p);
        tree::balanced_coarsen(static_cast<TreeGrid&>(*this), p);
      }
    }
    return p;
  }

  /// \name Tree-Node-to-Grid-Node map: (tree_node_idx, grid_idx) ->
  /// grid_node_idx
  /// transform
  ///@{

  /// Index of node \p n within grid \p g
  inline grid_node_idx node(tree_node_idx n, grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return grids_(*n, *g);
  }
  /// Index of node \p n within grid \p g
  inline grid_node_idx& node(tree_node_idx n, grid_idx g) noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return grids_(*n, *g);
  }

  /// Is the node \p n part of grid \p g ?
  inline bool in_grid(tree_node_idx n, grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return n ? (node(n, g) != grid_node_idx{}) : false;
  }

  /// Filters nodes that belong to the grid \p g
  inline auto in_grid(grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    return view::filter([&, g](tree_node_idx n) { return in_grid(n, g); });
  }

  /// Maps grid ids to grid node ids at a given tree node \p n
  inline auto to_grid_nodes(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return view::transform([&, n](grid_idx g) { return node(n, g); });
  }

  /// Maps tree node ids to grid node ids of grid \p g
  inline auto to_grid_nodes(grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    return view::transform(
     [&, g](tree_node_idx n) { return n ? node(n, g) : grid_node_idx{}; });
  }

  /// All nodes in grid \p g
  inline auto nodes(grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    return TreeGrid::nodes() | in_grid(g);
  }

  /// All grid nodes in grid \p g
  inline auto grid_nodes(grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    return nodes(g) | to_grid_nodes(g);
  }

  /// All grid nodes in node \p n (including invalid grid nodes)
  inline auto grid_nodes(tree_node_idx n) const noexcept {
    assert_node_in_use(n, HM3_AT_);
    return grids() | to_grid_nodes(n);
  }

  /// Children of node \p n in grid \p g
  inline auto children(tree_node_idx n, grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return TreeGrid::children(n) | in_grid(g) | to_grid_nodes(g);
  }

  /// Siblings of node \p n in grid \p g
  inline auto siblings(tree_node_idx n, grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return TreeGrid::siblings(n) | in_grid(g) | to_grid_nodes(g);
  }

  /// All neighbors (across all manifolds) of node \p n in grid \p g
  inline auto neighbors(tree_node_idx n, grid_idx g) const noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return tree::node_neighbors(*this, tree::node_location(*this, n),
                                [&, g](tree_node_idx i) {
                                  HM3_ASSERT(i, "");
                                  return in_grid(i, g);
                                });
  }

  /// All neighbors across \p manifold of node \p n in grid \p g
  template <typename Manifold>
  inline auto neighbors(tree_node_idx n, grid_idx g, Manifold manifold) const
   noexcept {
    assert_grid_in_bounds(g, HM3_AT_);
    assert_node_in_use(n, HM3_AT_);
    return tree::node_neighbors(manifold, *this, tree::node_location(*this, n),
                                [&, g](tree_node_idx i) {
                                  HM3_ASSERT(i, "");
                                  return in_grid(i, g);
                                });
  }

  ///@}  // Node-to-Grid-Node map
};

template <typename TreeGrid>
bool operator==(multi<TreeGrid> const& a, multi<TreeGrid> const& b) noexcept {
  using tree_grid_t = TreeGrid const&;
  return static_cast<tree_grid_t>(a) == static_cast<tree_grid_t>(b)
         && a.grids_() == b.grids_();
}

template <typename TreeGrid>
bool operator!=(multi<TreeGrid> const& a, multi<TreeGrid> const& b) noexcept {
  return !(a == b);
}

template <typename TreeGrid> string type(multi<TreeGrid> const&) {
  return "multi_" + type(TreeGrid{});
}

template <typename TreeGrid> string name(multi<TreeGrid> const&) {
  return "multi_" + name(TreeGrid{});
}

}  // namespace adaptor
}  // namespace grid
}  // namespace hm3
