#pragma once
/// \file
///
/// Finite-volume solver state
#include <hm3/solver/fv/tile.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Finite volume discretization state
template <typename TileLayout, typename Physics, typename TimeIntegration,
          typename NumericalFlux, typename Method>
struct state
 : grid_client_type<
    tile_type<TileLayout, Physics, TimeIntegration, NumericalFlux, Method>> {
  using self
   = state<TileLayout, Physics, TimeIntegration, NumericalFlux, Method>;
  using tile_t
   = tile_type<TileLayout, Physics, TimeIntegration, NumericalFlux, Method>;
  using grid_client_t = grid_client_type<tile_t>;
  using grid_t        = typename grid_client_t::grid_t;
  using tile_idx_t    = typename grid_client_t::tile_idx_t;

  using physics_t          = Physics;
  using time_integration_t = TimeIntegration;
  using numerical_flux_t   = NumericalFlux;
  using method_t           = Method;

  static constexpr uint_t nvars() noexcept { return physics_t::nvars(); }
  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }

  physics_t physics;
  time_integration_t time_integration;
  numerical_flux_t numerical_flux;
  method_t method;

  state(grid_t& g, grid_idx gidx, tile_idx_t tile_capacity, physics_t p)
   : grid_client_t(g, gidx, tile_capacity), physics(p) {}

  decltype(auto) variables(cell_idx c) const noexcept {
    return this->tile(c).variables(this->tile_local_cell_idx(c));
  }

  friend bool operator==(self const&, self const&) noexcept { return false; }

  friend bool operator!=(self const& a, self const& b) noexcept {
    return !(a == b);
  }
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
