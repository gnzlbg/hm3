#pragma once
/// \file
///
/// Tile right-hand-side cell variables
#include <hm3/solver/types.hpp>
#include <hm3/solver/utility/tile.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename Grid, vidx_t Nv, typename Order = dense::col_major_t>
struct right_hand_side {
  using grid_t   = Grid;
  using cell_idx = typename grid_t::cell_indices_t::coordinate;

  using storage_t
   = dense::matrix<num_t, grid_t::cells().size(), Nv, tidx_t, vidx_t, Order>;
  storage_t rhs_;

  auto& rhs() noexcept { return rhs_; }
  auto const& rhs() const noexcept { return rhs_; }
  auto rhs(cell_idx c) noexcept { return rhs_.row(*c.idx()); }
  auto rhs(cell_idx c) const noexcept { return rhs_.row(*c.idx()); }

  void clear() { rhs_ = storage_t::zero(); }
  void reinitialize() {}
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
