#pragma once
/// \file
///
/// Finite-volume solver state
#include <hm3/solver/fv/block.hpp>
#include <hm3/solver/utility/hierarchical_block_structured_grid.hpp>
// #include <hm3/solver/state/runge_kutta.hpp>
// #include <hm3/solver/state/boundary_conditions.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename Physics, typename TimeIntegration,
          typename block_base_t
          = block_base<Physics::dimension(), Physics::nvars(), 10, 2>>
using block_t_
 = block<block_base_t,
         decltype(TimeIntegration::state_t(std::declval<block_base_t>()))>;

/// Finite volume discretization state
template <typename Physics, typename TimeIntegration>  //
struct state
 : hierarchical_block_structured_grid<block_t_<Physics, TimeIntegration>> {
  using self = state<Physics, TimeIntegration>;
  using hbsg
   = hierarchical_block_structured_grid<block_t_<Physics, TimeIntegration>>;
  using grid_t = typename hbsg::grid_t;
  using geometry::dimensional<Physics::dimension()>::dimension;
  using geometry::dimensional<Physics::dimension()>::dimensions;

  static constexpr uint_t nvars() noexcept { return Physics::nvars(); }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }

  using physics_t = Physics;
  using block_idx = typename hbsg::block_idx;

  Physics physics;
  TimeIntegration time_integration;

  state(grid_t& g, grid_idx gidx, block_idx block_capacity, Physics p)
   : hbsg(g, gidx, block_capacity), physics(p) {}

  // void refine(block_idx bidx) {
  //   hbsg::refine(bidx, project_parent_to_children);
  // }
  // void coarsen(block_idx bidx) {
  //   hbsg::coarsen(bidx, restrict_children_to_parent);
  // }

  decltype(auto) variables(cell_idx c) const noexcept {
    return (this->block(this->block_i(c))).variables(this->block_cell_idx(c));
  }

  friend bool operator==(self const&, self const&) noexcept { return false; }

  friend bool operator!=(self const& a, self const& b) noexcept {
    return !(a == b);
  }
};

}  // namespace fv
}  // namespace name
}  // namespace hm3
