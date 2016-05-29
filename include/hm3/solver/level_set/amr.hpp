#pragma once
/// \file
///
/// Adaptive mesh refinement for the level-set solver
#include <hm3/grid/hierarchical/amr/amr.hpp>
#include <hm3/grid/hierarchical/types.hpp>
#include <hm3/solver/level_set/state.hpp>

namespace hm3 {
namespace solver {
namespace level_set {

/// Adaptive mesh refinement target for the level set solver
template <dim_t Nd> struct amr : geometry::dimensional<Nd> {
  using amr_node_idx = grid_node_idx;

  state<Nd>* ls_;

  amr(state<Nd>& s) : ls_{&s} {}

  /// Siblings of node \p n within the solver grid
  auto siblings(amr_node_idx n) const { return ls_->g.siblings(n); }
  /// Number of siblings within a node in the solver grid
  constexpr uint_t no_siblings() const noexcept {
    return ls_->g.tree().no_children();
  }

  /// Neighbors of node \p n in the solver grid
  auto neighbors(amr_node_idx n) const {
    return ls_->g.grid_node_neighbors_in_tree(n);
  }
  /// Level of node \p n
  auto level(amr_node_idx n) const { return ls_->g.level(n); }

  /// Refines node \p n in the solver grid
  void refine(amr_node_idx n) { ls_->refine(n); }

  /// Coarsen siblings of \p n in the solver grid
  void coarsen_siblings_of(amr_node_idx n) { ls_->coarsen(n); }

  /// Nodes of the level set solver grid
  auto nodes() const { return ls_->g.in_use(); }

  template <typename... Args> auto log(Args&&... args) const {
    return ls_->log(std::forward<Args>(args)...);
  }

  // These make the adaptive mesh refinement target serializable (e.g. useful
  // for debugging):

  auto bounding_box() const { return ls_->g.tree().bounding_box(); }
  auto geometry(amr_node_idx n) const {
    return ls_->g.tree().geometry(ls_->g.tree_node(n));
  }
};

/// Creates an adaptive mesh refinement handler for the level-set solver state
/// \p s
template <dim_t Nd>
::hm3::hierarchical::amr::state<amr<Nd>> make_amr(state<Nd>& s) noexcept {
  return amr<Nd>{s};
}

}  // namespace level_set
}  // namespace solver
}  // namespace hm3
