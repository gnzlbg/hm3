#pragma once
/// \file
///
/// Adaptive Mesh Refinement target for hierarchical::cartesian::multi<Ad>
#include <hm3/grid/hierarchical/amr/target.hpp>
#include <hm3/grid/hierarchical/cartesian/multi.hpp>
#include <hm3/grid/hierarchical/types.hpp>

namespace hm3::grid::hierarchical::cartesian {

template <dim_t Ad>
struct multi_amr_target : amr::non_loggable,
                          geometry::with_ambient_dimension<Ad> {
  using amr_node_idx = tree_node_idx;

  multi<Ad>* g_;

  multi_amr_target(multi<Ad>& g) : g_(&g) {}

  /// Siblings of node \p within the grid
  auto siblings(amr_node_idx n) const noexcept {
    return g_->siblings(n) | g_->leaf();
  }
  /// Max number of siblings at the same level within the grid
  constexpr cpidx_t no_siblings() const noexcept { return g_->no_children(); }

  /// Neighbors of node \p n within the grid
  auto neighbors(amr_node_idx n) const { return g_->neighbors(n); }
  /// Level of node \p n within the grid
  level_idx level(amr_node_idx n) const { return tree::node_level(*g_, n); }
  /// Coarsen siblings of node \p n within the grid
  void coarsen_siblings_of(amr_node_idx n) { g_->coarsen(g_->parent(n)); }
  /// Refine node \p n
  void refine(amr_node_idx n) { g_->refine(n); }
  /// Range of nodes to modify
  auto nodes() const { return g_->nodes() | g_->leaf(); }

  /// Required to model the SerializableToVTK concept (optional)
  /// (allows writing the AMR state to vtk for debugging purposes):
  geometry::box<Ad> geometry(amr_node_idx n) const { return g_->node(n); }
  geometry::box<Ad> bounding_box() const { return g_->bounding_box(); }
};

template <dim_t Ad>
multi_amr_target<Ad> make_amr_target(multi<Ad>& g) noexcept {
  return multi_amr_target<Ad>(g);
};

}  // namespace hm3::grid::hierarchical::cartesian
