#pragma once
/// \file
///
/// Lattice-Boltzmann solver state
#include <hm3/solver/lbm/tile.hpp>
#include <hm3/solver/utility/base.hpp>
#include <hm3/solver/utility/tiled_hierarchical_grid.hpp>

namespace hm3::solver::lbm {

template <typename Physics, dim_t Nic = 12>
struct state : tiled_hierarchical_grid<tile<Physics, Nic>>, base {
  using tile_t        = tile<Physics, Nic>;
  using grid_client_t = tiled_hierarchical_grid<tile_t>;
  using self          = state<Physics>;
  using tile_idx_t    = typename grid_client_t::tile_idx_t;
  using grid_t        = typename grid_client_t::grid_t;

  Physics physics;

  state(grid_t& g, grid_idx gidx, tile_idx_t tile_capacity, Physics p)
   : grid_client_t(g, gidx, tile_capacity)
   , base(g.client().session(), gidx, *this, this->tree())
   , physics(p) {}

  decltype(auto) distributions0(cell_idx_t c, uint_t d) const noexcept {
    return (this->tile(this->tile_idx(c)))
     .distribution0(this->tile_local_cell_idx(c), d);
  }

  decltype(auto) distributions1(cell_idx_t c, uint_t d) const noexcept {
    return (this->tile(this->tile_idx(c)))
     .distribution1(this->tile_local_cell_idx(c), d);
  }

  friend bool operator==(self const&, self const&) noexcept { return false; }

  friend bool operator!=(self const& a, self const& b) noexcept {
    return !(a == b);
  }
};

template <typename Physics, dim_t N>
string type(state<Physics, N> const&) {
  return "lbm_" + Physics::name();
}

}  // namespace hm3::solver::lbm
