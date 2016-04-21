#pragma once
/// \file
///
/// Square halo tile geometry
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/grid/structured/halo_tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/geometry.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {

/// Square structured halo tile spatial information
///
/// Note: external and internal are used to talk about the tile geometry of the
/// tile with and without halo cells, respectively.
///
/// \tparam Nd  number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <dim_t Nd, suint_t Nic, suint_t Nhl>  //
struct tile_geometry
 : private tile::tile_geometry<Nd, cell::bounds<Nd, Nic, Nhl>::length()> {
  using bounds                 = cell::bounds<Nd, Nic, Nhl>;
  using external_tile_geometry = tile::tile_geometry<Nd, bounds::length()>;
  using internal_tile_geometry = tile::tile_geometry<Nd, Nic>;
  using cell_coordinate        = cell::coordinate<Nd, Nic, Nhl>;
  using point_t                = typename external_tile_geometry::point_t;
  using square_t               = typename external_tile_geometry::square_t;

  // imports from external_tile_geometry
  using external_tile_geometry::cell_length;
  using external_tile_geometry::cell_volume;
  using external_tile_geometry::cell_surface_area;
  using external_tile_geometry::tile_center;
  using external_tile_geometry::contains;
  using external_tile_geometry::cell_containing;

  constexpr tile_geometry()                     = default;
  constexpr tile_geometry(tile_geometry const&) = default;
  constexpr tile_geometry(tile_geometry&&)      = default;
  constexpr tile_geometry& operator=(tile_geometry const&) = default;
  constexpr tile_geometry& operator=(tile_geometry&&) = default;

  /// Construct a halo tile geometry from the bounding box of the internal cells
  static constexpr tile_geometry from(square_t internal_bbox) noexcept {
    auto cell_length = internal_tile_geometry::cell_length(internal_bbox);
    auto external_tile_length          = cell_length * bounds::length();
    auto external_tile_bounding_box    = internal_bbox;
    external_tile_bounding_box.length_ = external_tile_length;
    return tile_geometry(external_tile_geometry(external_tile_bounding_box));
  }

  /// Constructs a halo tile geometry from the bounding box of the internal
  /// cells
  constexpr tile_geometry(square_t internal_bbox)
   : tile_geometry(from(internal_bbox)) {}
  /// Constructs a halo tile geometry from a non-halo tile geometry (the same
  /// bounding box is used)
  constexpr tile_geometry(external_tile_geometry g)
   : external_tile_geometry(std::move(g)) {}

  /// Internal cell containing the point \p x
  ///
  /// \warning returns invalid coordinate if \p x doesn't lie in the internal
  /// part of the tile
  constexpr cell_coordinate internal_cell_containing(point_t x) const noexcept {
    cell_coordinate x_c(external_tile_geometry::cell_containing(x));
    if (x_c and x_c.is_internal()) { return x_c; }
    return cell_coordinate();
  }

  /// Halo cell containing the point \p x
  ///
  /// \warning returns invalid coordinate if \p x doesn't lie in the halo
  /// part of the tile
  constexpr cell_coordinate halo_cell_containing(point_t x) const noexcept {
    cell_coordinate x_c(external_tile_geometry::cell_containing(x));
    if (x_c and x_c.is_halo()) { return x_c; }
    return cell_coordinate();
  }

  /// Length of the tile without halo cells (only internal cells)
  constexpr num_t tile_internal_length() const noexcept {
    return tile_external_length() - 2 * Nhl * this->cell_length();
  }

  /// Length of the tile with halo cells
  constexpr num_t tile_external_length() const noexcept {
    return static_cast<external_tile_geometry const&>(*this).tile_length();
  }

  /// Bounding box of the tile without halo cells
  constexpr square_t tile_internal_bounding_box() const noexcept {
    auto internal_bbox    = tile_external_bounding_box();
    internal_bbox.length_ = tile_internal_length();
    return internal_bbox;
  }

  /// Bounding box of the tile with halo cells
  constexpr square_t tile_external_bounding_box() const noexcept {
    return static_cast<external_tile_geometry const&>(*this)
     .tile_bounding_box();
  }
};

}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
