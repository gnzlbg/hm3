/// \file
///
/// Tile cell gradients
#include <hm3/solver/types.hpp>
#include <hm3/solver/utility/tile.hpp>

namespace hm3::solver::fv {

template <typename Grid, vidx_t Nv, typename Order = dense::col_major_t>
struct cell_gradients {
  using grid_t   = Grid;
  using cell_idx = typename grid_t::cell_indices_t::coordinate;
  using storage_t
   = dense::matrix<num_t, grid_t::cells().size(),
                   Nv * Grid::ambient_dimension(), tidx_t, vidx_t, Order>;
  storage_t gradient_;

  auto& gradient() noexcept { return gradient_; }
  auto const& gradient() const noexcept { return gradient_; }

  auto gradient(cell_idx c, dim_t d) noexcept {
    return gradient_.row(*c.idx()).template segment<Nv>(d * Nv);
  }
  auto gradient(cell_idx c, dim_t d) const noexcept {
    return gradient_.row(*c.idx()).template segment<Nv>(d * Nv);
  }

  void clear() { gradient_ = storage_t::zero(); }
  void reinitialize() {}
};

}  // namespace hm3::solver::fv
