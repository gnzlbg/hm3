#pragma once
/// \file
///
/// Lattice-Boltzmann solver state
#include <hm3/solver/utility/hierarchical_block_structured_grid.hpp>
#include <hm3/solver/lbm/block.hpp>

namespace hm3 {
namespace solver {
namespace lbm {

template <typename Lattice>
using block_t_ = block<Lattice::dimension(), Lattice::size(), 100, 2>;

template <typename Lattice>  //
struct state : hierarchical_block_structured_grid<block_t_<Lattice>> {
  using block_t   = block_t_<Lattice>;
  using hbsg      = hierarchical_block_structured_grid<block_t>;
  using self      = state<Lattice>;
  using grid_t    = typename hbsg::grid_t;
  using lattice_t = Lattice;
  using block_idx = typename hbsg::block_idx;

  state(grid_t& g, grid_idx gidx, block_idx block_capacity)
   : hbsg(g, gidx, block_capacity) {}

  // void refine(block_idx bidx) {
  //   hbsg::refine(bidx, project_parent_to_children);
  // }
  // void coarsen(block_idx bidx) {
  //   hbsg::coarsen(bidx, restrict_children_to_parent);
  // }

  decltype(auto) nodes0(cell_idx c, uint_t d) const noexcept {
    return (this->block(this->block(c))).nodes0(this->block_cell_idx(c), d);
  }

  friend bool operator==(self const&, self const&) noexcept { return false; }

  friend bool operator!=(self const& a, self const& b) noexcept {
    return !(a == b);
  }
};

}  // namespace lbm
}  // namespace solver
}  // namespace hm3
