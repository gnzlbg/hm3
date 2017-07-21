#pragma once
/// \file
///
/// Square halo tile geometry
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/grid/structured/halo_tile/cell/coordinate.hpp>
#include <hm3/grid/structured/halo_tile/cell/indices.hpp>
#include <hm3/grid/structured/tile/geometry.hpp>

namespace hm3::grid::structured::halo_tile {

template <dim_t Ad, tidx_t Nic, tidx_t Nhl>
struct cell_geometry : geometry::box<Ad>,
                       cell::indices<Ad, Nic, Nhl>::coordinate {
  using box_t         = geometry::box<Ad>;
  using coordinate_t  = typename cell::indices<Ad, Nic, Nhl>::coordinate;
  using geometry_type = typename box_t::geometry_type;

  cell_geometry(box_t s, coordinate_t x)
   : box_t{std::move(s)}, coordinate_t(std::move(x)) {}
  explicit operator box_t() const noexcept {
    return static_cast<box_t const&>(*this);
  }
  explicit operator coordinate_t() const noexcept {
    return static_cast<coordinate_t const&>(*this);
  }
};

static_assert(geometry::Box<cell_geometry<2, 2, 2>>{});  // TODO: remove

/// Box structured halo tile spatial information
///
/// Note: external and internal are used to talk about the tile geometry of the
/// tile with and without halo cells, respectively.
///
/// \tparam Ad  number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <dim_t Ad, tidx_t Nic, tidx_t Nhl>
struct tile_geometry
 : private tile::tile_geometry<Ad, cell::bounds<Ad, Nic, Nhl>::length()> {
  using bounds                 = cell::bounds<Ad, Nic, Nhl>;
  using external_tile_geometry = tile::tile_geometry<Ad, bounds::length()>;
  using cell_coordinate        = cell::coordinate<Ad, Nic, Nhl>;
  using point_t                = typename external_tile_geometry::point_t;
  using box_t                  = typename external_tile_geometry::box_t;
  using cell_geometry_t        = cell_geometry<Ad, Nic, Nhl>;

  // imports from external_tile_geometry
  using external_tile_geometry::cell_centroid;
  using external_tile_geometry::cell_containing;
  using external_tile_geometry::cell_length;
  using external_tile_geometry::cell_surface_area;
  using external_tile_geometry::cell_volume;
  using external_tile_geometry::contains;
  using external_tile_geometry::tile_centroid;

  constexpr tile_geometry()                     = default;
  constexpr tile_geometry(tile_geometry const&) = default;
  constexpr tile_geometry(tile_geometry&&)      = default;
  constexpr tile_geometry& operator=(tile_geometry const&) = default;
  constexpr tile_geometry& operator=(tile_geometry&&) = default;

  static constexpr box_t external_bounding_box(box_t internal_bbox) noexcept {
    using internal_tile_geometry = tile::tile_geometry<Ad, Nic>;
    auto cell_length = internal_tile_geometry::cell_length(internal_bbox);
    auto external_tile_length = cell_length * bounds::length();
    auto external_tile_bounding_box
     = box_t{geometry::centroid(internal_bbox), external_tile_length};
    return external_tile_bounding_box;
  }

  /// Construct a halo tile geometry from the bounding box of the internal cells
  static constexpr tile_geometry from(box_t internal_bbox) noexcept {
    return tile_geometry(
     external_tile_geometry(external_bounding_box(std::move(internal_bbox))));
  }

  /// Constructs a halo tile geometry from the bounding box of the internal
  /// cells
  constexpr tile_geometry(box_t internal_bbox)
   : tile_geometry(from(internal_bbox)) {}
  /// Constructs a halo tile geometry from a non-halo tile geometry (the same
  /// bounding box is used)
  constexpr tile_geometry(external_tile_geometry g)
   : external_tile_geometry(std::move(g)) {}

  /// Updates the external bounding box (up to the halos)
  constexpr void set_external_bounding_box(box_t bbox) noexcept {
    this->set_bounding_box(std::move(bbox));
    HM3_ASSERT(geometry::approx(tile_external_bounding_box(), bbox),
               "tile external bbox: {}, bbox: {}", tile_external_bounding_box(),
               bbox);
  }

  // Updates the internal bounding box (without the halos)
  constexpr void set_internal_bounding_box(box_t bbox) noexcept {
    set_external_bounding_box(external_bounding_box(std::move(bbox)));
    HM3_ASSERT(geometry::approx(tile_internal_bounding_box(), bbox),
               "tile internal bbox: {}, bbox: {}", tile_internal_bounding_box(),
               bbox);
  }

  /// Internal cell containing the point \p x
  ///
  /// \warning returns invalid coordinate if \p x doesn't lie in the internal
  /// part of the tile
  constexpr cell_coordinate internal_cell_containing(point_t x) const noexcept {
    auto tmp = cell_containing(x);
    if (tmp) {
      cell_coordinate x_c(tmp);
      if (x_c.is_internal()) { return x_c; }
    }
    return cell_coordinate();
  }

  /// Halo cell containing the point \p x
  ///
  /// \warning returns invalid coordinate if \p x doesn't lie in the halo
  /// part of the tile
  constexpr cell_coordinate halo_cell_containing(point_t x) const noexcept {
    auto tmp = cell_containing(x);
    if (tmp) {
      cell_coordinate x_c(tmp);
      if (x_c.is_halo()) { return x_c; }
    }
    return cell_coordinate();
  }

  /// Length of the tile without halo cells (only internal cells)
  constexpr num_t tile_internal_length() const noexcept {
    return tile_external_length() - 2 * Nhl * this->cell_length();
  }

  /// Length of the tile with halo cells
  constexpr num_t tile_external_length() const noexcept {
    return this->tile_length();
  }

  /// Bounding box of the tile without halo cells
  ///
  /// TODO rename: remove tile_ ?
  constexpr box_t tile_internal_bounding_box() const noexcept {
    return box_t{geometry::centroid(tile_external_bounding_box()),
                 tile_internal_length()};
  }

  /// Bounding box of the tile with halo cells
  ///
  /// TODO rename: remove tile_ ?
  constexpr box_t tile_external_bounding_box() const noexcept {
    return this->tile_bounding_box();
  }

  /// Geometry of cell at coordinate \p x
  constexpr cell_geometry_t operator()(cell_coordinate x) const noexcept {
    using x_t = typename external_tile_geometry::cell_coordinate;
    return cell_geometry_t{
     external_tile_geometry::bounding_box(static_cast<x_t>(x)), x};
  }
};

}  // namespace hm3::grid::structured::halo_tile
