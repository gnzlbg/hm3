#pragma once
/// \file
///
/// Square structured halo tile
///
/// TODO: should switch internaly to tile implementations if the number of halo
/// layers is zero
#include <hm3/grid/structured/halo_tile/cell.hpp>
#include <hm3/grid/structured/halo_tile/geometry.hpp>
#include <hm3/grid/structured/halo_tile/surface.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {

/// Square structured halo tile cell
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <dim_t Nd, tidx_t Nic, tidx_t Nhl>
struct tile : geometry::with_ambient_dimension<Nd> {
  using cell_indices_t       = cell::indices<Nd, Nic, Nhl>;
  using cell_coordinate_t    = typename cell_indices_t::coordinate;
  using surface_indices_t    = surface::indices<Nd, Nic, Nhl>;
  using surface_coordinate_t = typename surface_indices_t::coordinate;
  using geometry_t           = tile_geometry<Nd, Nic, Nhl>;
  using box_t                = typename geometry_t::box_t;

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

  constexpr tile()            = default;
  constexpr tile(tile const&) = default;
  constexpr tile& operator=(tile const&) = default;
  constexpr tile(tile&&)                 = default;
  constexpr tile& operator=(tile&&) = default;

  constexpr tile(box_t bbox) : geometry_(std::move(bbox)) {}
};

}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
