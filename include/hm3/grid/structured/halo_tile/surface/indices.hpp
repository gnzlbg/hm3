#pragma once
/// \file
///
/// Structured square tile's surface indices
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/grid/structured/halo_tile/cell/indices.hpp>
#include <hm3/grid/structured/tile/surface/indices.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {

namespace surface {

/// Square structured halo tile surface indices
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <dim_t Nd, tidx_t Nic, tidx_t Nhl>  //
struct indices
 : tile::surface::indices<Nd, cell::bounds<Nd, Nic, Nhl>::size()> {
  using self            = indices<Nd, Nic, Nhl>;
  using cell_bounds     = cell::bounds<Nd, Nic, Nhl>;
  using cell_indices    = cell::indices<Nd, Nic, Nhl>;
  using cell_coordinate = typename cell_indices::coordinate;

  /// For each surface of the internal cells in the tile with normal component
  /// in direction \p d
  template <typename F>
  static constexpr void for_each_internal(F&& f, dim_t d) noexcept {
    self::for_each(cell_coordinate::constant(cell_bounds::first_internal()),
                   cell_coordinate::constant(cell_bounds::last_internal()),
                   std::forward<F>(f), d);
  }

  /// For each surface of the internal cells in the tile
  template <typename F>
  [[ HM3_FLATTEN, HM3_ALWAYS_INLINE, HM3_HOT ]]  //
   static constexpr void
   for_each_internal(F&& f) noexcept {
    self::for_each(cell_coordinate::constant(cell_bounds::first_internal()),
                   cell_coordinate::constant(cell_bounds::last_internal()),
                   std::forward<F>(f));
  }
};

}  // namespace surface

}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
