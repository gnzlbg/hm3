#pragma once
/// \file
///
/// Stores a single grid within a hierarchical Cartesian multi-tree and
/// internally stores grid neighbors
///
/// TODO: allow selecting the set of neighbors to be stored (right now neighbors
/// across all manifolds are stored)
///
/// TODO: experiment with storing only extra neighbors (e.g. ghost nodes) while
/// obtaining the other neighbors dynamically from the grid
#include <hm3/grid/hierarchical/client/multi.hpp>
#include <hm3/grid/hierarchical/client/neighbor_storage.hpp>
namespace hm3 {
namespace grid {
namespace hierarchical {
namespace client {

/// Stores a single grid within a hierarchical Cartesian multi-tree and
/// internally stores grid neighbors
///
/// \tparam Nd number of spatial dimensions of the grid
///
/// This store a grid client and a internal copy of neighbors across all
/// manifolds.
template <dim_t Nd>  //
struct multi_wn : geometry::dimensional<Nd> {
  using self  = multi_wn<Nd>;
  using multi = multi<Nd>;
  using neighbor_storage
   = neighbor_storage<tree::max_no_neighbors(Nd) + 1>;

  multi grid;
  neighbor_storage neighbors;

  multi_wn()                = default;
  multi_wn(multi_wn const&) = default;
  multi_wn(multi_wn&&)      = default;
  multi_wn& operator=(multi_wn const&) = default;
  multi_wn& operator=(multi_wn&&) = default;

  multi_wn(multi g) : grid(std::move(g)), neighbors(grid.capacity()) {}

  /// Append the tree node \p n into the grid and returns its grid node idx
  grid_node_idx push(tree_node_idx n) {
    auto i = grid.push(n);
    neighbors.push(i, grid.neighbors(i));
    return i;
  }

  /// Remove the grid node \p n from the grid
  void pop(grid_node_idx n) {
    grid.pop(n);
    neighbors.pop(n);
  }

  /// Refines node \p p () using a \p projection.
  ///
  /// \tparam Projection A BinaryFunction(parent, new_children_range) used to to
  /// project data from the parent onto its new children.
  ///
  /// \note This functions removes the parent node \p p from the grid.
  template <typename Projection>
  void refine(grid_node_idx parent, Projection&& projection) {
    auto refine_guard = grid.refine(parent);
    projection(refine_guard.old_parent(), refine_guard.new_children());
    neighbors.pop(parent);  // remove parent from the grid first
    for (auto c : refine_guard.new_children()) {
      neighbors.push(c, grid.neighbors(c));
    }
    /// \note parent node `parent` is removed on refine guard destruction
  }

  /// Coarsens the parent of the node \p child using a \p restriction.
  ///
  /// \tparam Restriction A BinaryFunction(parent, old_children_range) used to
  /// restrict data from the old children into the new parent.
  ///
  /// \note This function removes all siblings of \p child from the grid (that
  /// is, all children of the newly created parent of \p child).
  template <typename Restriction>
  void coarsen(grid_node_idx child, Restriction&& restriction) {
    auto coarsen_guard = grid.coarsen(child);
    restriction(coarsen_guard.new_parent(), coarsen_guard.old_children());
    for (auto c : coarsen_guard.old_children()) { neighbors.pop(c); }
    neighbors.push(coarsen_guard.new_parent());
    /// \note siblings of node `child` are removed on coarsen guard destruction
  }
};

}  // namespace client
}  // namespace hierarchical
}  // namespace grid
}  // namespace hm3
