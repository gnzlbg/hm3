#pragma once
/// \file
///
/// Square structured tile
#include <hm3/grid/structured/tile/cell.hpp>
#include <hm3/grid/structured/tile/geometry.hpp>
#include <hm3/grid/structured/tile/surface.hpp>

namespace hm3::grid::structured::tile {

/// Square structured tile
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc length (per dimension)
template <dim_t Ad, tidx_t Nc>
struct tile : geometry::with_ambient_dimension<Ad> {
  using cell_indices_t    = cell::indices<Ad, Nc>;
  using surface_indices_t = surface::indices<Ad, Nc>;
  using geometry_t        = tile_geometry<Ad, Nc>;

  geometry_t geometry_;

  /// Tile geometry:
  constexpr geometry_t const& geometry() const noexcept { return geometry_; }
  constexpr geometry_t& geometry() noexcept { return geometry_; }
  /// Tile cell indices:
  static constexpr cell_indices_t cells() noexcept { return cell_indices_t{}; }
  /// Tile surface indices:
  static constexpr surface_indices_t surfaces() noexcept {
    return surface_indices_t{};
  }
};

}  // namespace hm3::grid::structured::tile
