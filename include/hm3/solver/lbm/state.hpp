#ifdef FIXED
#pragma once
/// \file
///
/// Lattice-Boltzmann solver state
#include <hm3/solver/lbm/block.hpp>
#include <hm3/solver/utility/hierarchical_block_structured_grid.hpp>

namespace hm3::solver::lbm {

template <typename Physics>
using block_t_ = block<Physics::ambient_dimension(), Physics::size(), 128, 0>;

template <typename Physics>
struct state : hierarchical_block_structured_grid<block_t_<Physics>> {
  using block_t   = block_t_<Physics>;
  using hbsg      = hierarchical_block_structured_grid<block_t>;
  using self      = state<Physics>;
  using grid_t    = typename hbsg::grid_t;
  using block_idx = typename hbsg::block_idx;

  Physics physics;

  state(grid_t& g, grid_idx gidx, block_idx block_capacity, Physics p)
   : hbsg(g, gidx, block_capacity), physics(p) {}

  // void refine(block_idx bidx) {
  //   hbsg::refine(bidx, project_parent_to_children);
  // }
  // void coarsen(block_idx bidx) {
  //   hbsg::coarsen(bidx, restrict_children_to_parent);
  // }

  decltype(auto) nodes0(cell_idx c, uint_t d) const noexcept {
    return (this->block(this->block_i(c))).nodes0(this->block_cell_idx(c), d);
  }

  decltype(auto) nodes1(cell_idx c, uint_t d) const noexcept {
    return (this->block(this->block_i(c))).nodes1(this->block_cell_idx(c), d);
  }

  friend bool operator==(self const&, self const&) noexcept { return false; }

  friend bool operator!=(self const& a, self const& b) noexcept {
    return !(a == b);
  }
};

}  // namespace hm3::solver::lbm
#endif
