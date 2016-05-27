#pragma once
/// \file
///
/// Tile left-hand-side cell variables
#include <hm3/solver/types.hpp>
#include <hm3/solver/utility/tile.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename Grid, vidx_t Nv, typename Order = dense::col_major_t>
struct left_hand_side {
  using grid_t   = Grid;
  using cell_idx = typename grid_t::cell_indices_t::coordinate;

  using storage_t
   = dense::matrix<num_t, grid_t::cells().size(), Nv, tidx_t, vidx_t, Order>;
  storage_t lhs_;

  auto& lhs() noexcept { return lhs_; }
  auto const& lhs() const noexcept { return lhs_; }
  auto lhs(cell_idx c) noexcept { return lhs_.row(*c.idx()); }
  auto lhs(cell_idx c) const noexcept { return lhs_.row(*c.idx()); }

  auto& variables() noexcept { return lhs(); }
  auto const& variables() const noexcept { return lhs(); }
  auto variables(cell_idx c) noexcept { return lhs(c); }
  auto variables(cell_idx c) const noexcept { return lhs(c); }

  void clear() { lhs_ = storage_t::zero(); }
  void reinitialize() {}
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
