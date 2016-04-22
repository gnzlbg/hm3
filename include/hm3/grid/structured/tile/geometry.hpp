#pragma once
/// \file
///
/// Square tile geometry
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/grid/structured/tile/cell.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {

/// Square structured tile spatial information
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc length (per dimension)
template <dim_t Nd, tidx_t Nc>  //
struct tile_geometry : geometry::dimensional<Nd> {
  using bounds          = cell::bounds<Nd, Nc>;
  using point_t         = geometry::point<Nd>;
  using square_t        = geometry::square<Nd>;
  using cell_indices    = cell::indices<Nd, Nc>;
  using cell_coordinate = typename cell_indices::coordinate;

  point_t x_first_cell_;
  num_t cell_length_;

  /// Cell length
  constexpr num_t cell_length() const noexcept { return cell_length_; }

  /// First cell center coordinates
  constexpr point_t x_first_cell() const noexcept { return x_first_cell_; }

  /// Cell volume
  constexpr num_t cell_volume() const noexcept {
    return std::pow(cell_length(), Nd);
  }

  /// Cell surface area
  constexpr num_t cell_surface_area() const noexcept {
    return std::pow(cell_length(), Nd - 1);
  }

  /// Tile length
  constexpr num_t tile_length() const noexcept {
    return cell_length() * num_t{bounds::length()};
  }

  /// Tile center
  constexpr point_t tile_center(num_t tile_length) const noexcept {
    point_t x_tile_center = x_first_cell();
    const auto tl         = tile_length;
    const auto l          = cell_length();
    for (dim_t d = 0; d < Nd; ++d) { x_tile_center[d] += 0.5 * (tl - l); }
    return x_tile_center;
  }
  /// Tile center
  constexpr point_t tile_center() const noexcept {
    return tile_center(tile_length());
  }

  /// Tile cell center
  constexpr point_t cell_center(cell_coordinate x) const noexcept {
    HM3_ASSERT(x, "cannot compute center of invalid coordinate {}", x);
    point_t r    = x_first_cell();
    const auto l = cell_length();
    for (dim_t d = 0; d < Nd; ++d) { r[d] += x[d] * l; }

    return r;
  }

  /// Tile bounding box
  constexpr square_t tile_bounding_box() const noexcept {
    num_t tile_length     = this->tile_length();
    point_t x_tile_center = tile_center(tile_length);
    return square_t{x_tile_center, tile_length};
  }

  /// Compute the cell length from the length of the tile
  static constexpr num_t cell_length(num_t tile_length) noexcept {
    HM3_ASSERT(tile_length > 0., "tile length {} must be positive",
               tile_length);
    return tile_length / bounds::length();
  }

  /// Compute the cell length from the tile bounding box
  static constexpr num_t cell_length(square_t bbox) noexcept {
    return cell_length(geometry::length(bbox));
  }

  /// Computes first cell center coordinates (idx = 0) from tile bounding box
  static constexpr point_t x_first_cell(square_t bbox) noexcept {
    HM3_ASSERT(geometry::length(bbox) > 0.,
               "bounding box length must be positive, bbox {}", bbox);
    auto x_min  = geometry::bounds(bbox).min;
    auto cell_l = cell_length(geometry::length(bbox));
    for (dim_t d = 0; d < Nd; ++d) { x_min(d) += cell_l / 2.; }
    return x_min;
  }

  /// Lies point \p x inside the tile?
  constexpr bool contains(point_t x) const noexcept {
    return geometry::contains(tile_bounding_box(), x);
  }

  /// Cell containing the point \p x
  ///
  /// \warning returns invalid coordinate if \p x doesn't lie in the tile
  constexpr cell_coordinate cell_containing(point_t x) const noexcept {
    // normalize x: x_norm = (x - x_first) / cell_length:
    x() -= x_first_cell_();
    x() /= cell_length();

    cell_coordinate i;
    for (dim_t d = 0; d < Nd; ++d) {
      auto v = x[d];
      i[d]   = std::lround(v);
    }
    return i;
  }

  /// Returns a tile geometry from the tile's bounding box
  static constexpr tile_geometry from(square_t bounding_box) noexcept {
    tile_geometry g;
    g.cell_length_  = tile_geometry::cell_length(bounding_box);
    g.x_first_cell_ = tile_geometry::x_first_cell(bounding_box);
    return g;
  }

  constexpr tile_geometry()                     = default;
  constexpr tile_geometry(tile_geometry const&) = default;
  constexpr tile_geometry(tile_geometry&&)      = default;
  constexpr tile_geometry& operator=(tile_geometry const&) = default;
  constexpr tile_geometry& operator=(tile_geometry&&) = default;

  /// Constructs a tile geometry from its bounding box
  constexpr tile_geometry(square_t bounding_box)
   : tile_geometry(from(std::move(bounding_box))) {}
};

}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
