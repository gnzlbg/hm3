#pragma once
/// \file
///
/// Square structured tile
#include <hm3/grid/structured/tile/cell.hpp>
#include <hm3/grid/structured/tile/geometry.hpp>
#include <hm3/grid/structured/tile/surface.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {

/// Square structured tile
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc length (per dimension)
template <dim_t Nd, tidx_t Nc>
struct tile : geometry::dimensional<Nd> {
  using cell_indices_t    = cell::indices<Nd, Nc>;
  using surface_indices_t = surface::indices<Nd, Nc>;
  using geometry_t        = tile_geometry<Nd, Nc>;

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

}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
