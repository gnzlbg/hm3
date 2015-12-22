#pragma once
/// \file
///
/// Adaptive Mesh Refinement target for hc::multi<Nd>
#include <hm3/amr/target.hpp>
#include <hm3/grid/hc/multi.hpp>
#include <hm3/grid/types.hpp>

namespace hm3 {
namespace grid {
namespace hc {

template <uint_t Nd>
struct multi_amr_target : amr::non_loggable, geometry::dimensional<Nd> {
  using amr_node_idx = tree_node_idx;

  multi<Nd>* g_;

  multi_amr_target(multi<Nd>& g) : g_(&g) {}

  /// Siblings of node \p within the grid
  auto siblings(amr_node_idx n) const noexcept {
    return g_->siblings(n) | g_->leaf();
  }
  /// Max number of siblings at the same level within the grid
  constexpr int_t no_siblings() const noexcept { return g_->no_children(); }

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
  geometry::square<Nd> geometry(amr_node_idx n) const { return g_->node(n); }
  geometry::square<Nd> bounding_box() const { return g_->bounding_box(); }
};

template <uint_t Nd>
multi_amr_target<Nd> make_amr_target(multi<Nd>& g) noexcept {
  return multi_amr_target<Nd>(g);
};

}  // namespace hc

}  // namespace grid
}  // namespace hm3
