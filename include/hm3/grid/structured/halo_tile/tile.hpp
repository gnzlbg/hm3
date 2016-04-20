#pragma once
/// \file
///
/// Square structured halo tile
#include <hm3/grid/structured/halo_tile/cell.hpp>
#include <hm3/grid/structured/halo_tile/surface.hpp>
#include <hm3/grid/structured/tile/geometry.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {

/// Square structured halo tile cell
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct tile : geometry::dimensional<Nd> {
  using cell_indices_t    = cell::indices<Nd, Nic, Nhl>;
  using surface_indices_t = surface::indices<Nd, Nic, Nhl>;
  using geometry_t        = tile::geometry<Nd, Nc>;

  geometry_t geometry_;

  /// Tile geometry:
  constexpr geometry_t geometry() const noexcept { return geometry_; }
  /// Tile cell indices:
  static constexpr cell_indices_t cells() noexcept { return cell_indices_t{}; }
  /// Tile surface indices:
  static constexpr surface_indices_t surfaces() noexcept {
    return surface_indices_t{};
  }
};

}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
