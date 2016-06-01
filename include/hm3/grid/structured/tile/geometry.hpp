#pragma once
/// \file
///
/// Square tile geometry
#include <hm3/geometry/box.hpp>
#include <hm3/geometry/dimensions.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/grid/structured/tile/cell.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {

template <dim_t Nd, tidx_t Nc>  //
struct cell_geometry : geometry::box<Nd>, cell::indices<Nd, Nc>::coordinate {
  using box_t        = geometry::box<Nd>;
  using coordinate_t = typename cell::indices<Nd, Nc>::coordinate;
  cell_geometry(box_t s, coordinate_t x)
   : box_t{std::move(s)}, coordinate_t(std::move(x)) {}
  explicit operator box_t() const noexcept {
    return static_cast<box_t const&>(*this);
  }
  explicit operator coordinate_t() const noexcept {
    return static_cast<coordinate_t const&>(*this);
  }
};

/// Box structured tile spatial information
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc length (per dimension)
template <dim_t Nd, tidx_t Nc>  //
struct tile_geometry : geometry::dimensional<Nd> {
  using bounds          = cell::bounds<Nd, Nc>;
  using point_t         = geometry::point<Nd>;
  using box_t           = geometry::box<Nd>;
  using cell_indices    = cell::indices<Nd, Nc>;
  using cell_coordinate = typename cell_indices::coordinate;
  using cell_geometry_t = cell_geometry<Nd, Nc>;

  num_t cell_length_;
  point_t x_first_cell_;

  /// Cell length
  constexpr num_t cell_length() const noexcept { return cell_length_; }

  /// First cell centroid coordinates
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

  /// Tile centroid
  constexpr point_t tile_centroid(num_t tile_length) const noexcept {
    point_t x_tile_centroid = x_first_cell();
    const auto tl           = tile_length;
    const auto l            = cell_length();
    for (dim_t d = 0; d < Nd; ++d) { x_tile_centroid[d] += 0.5 * (tl - l); }
    return x_tile_centroid;
  }
  /// Tile centroid
  constexpr point_t tile_centroid() const noexcept {
    return tile_centroid(tile_length());
  }

  /// Tile cell centroid
  constexpr point_t cell_centroid(cell_coordinate x) const noexcept {
    HM3_ASSERT(x, "cannot compute centroid of invalid coordinate {}", x);
    point_t r    = x_first_cell();
    const auto l = cell_length();
    for (dim_t d = 0; d < Nd; ++d) { r[d] += x[d] * l; }

    return r;
  }

  /// Tile bounding box
  constexpr box_t tile_bounding_box() const noexcept {
    num_t tile_length       = this->tile_length();
    point_t x_tile_centroid = tile_centroid(tile_length);
    return box_t{x_tile_centroid, tile_length};
  }

  /// Compute the cell length from the length of the tile
  static constexpr num_t cell_length(num_t tile_length) noexcept {
    HM3_ASSERT(tile_length > 0., "tile length {} must be positive",
               tile_length);
    return tile_length / bounds::length();
  }

  /// Compute the cell length from the tile bounding box
  static constexpr num_t cell_length(box_t bbox) noexcept {
    return cell_length(geometry::length(bbox));
  }

  /// Bounding box of cell at coordinate \p x
  constexpr box_t bounding_box(cell_coordinate x) const noexcept {
    return box_t{cell_centroid(x), cell_length()};
  }

  /// Geometry of cell at coordinate \p x
  constexpr cell_geometry_t operator()(cell_coordinate x) const noexcept {
    return cell_geometry_t{bounding_box(x), x};
  }

  /// Computes first cell centroid coordinates (idx = 0) from tile bounding box
  static constexpr point_t x_first_cell(box_t bbox) noexcept {
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

  /// Sets the bounding box of the tile to \p bounding_box
  constexpr void set_bbox(box_t bounding_box) noexcept {
    cell_length_  = cell_length(bounding_box);
    x_first_cell_ = x_first_cell(bounding_box);
  }

  constexpr tile_geometry()                     = default;
  constexpr tile_geometry(tile_geometry const&) = default;
  constexpr tile_geometry(tile_geometry&&)      = default;
  constexpr tile_geometry& operator=(tile_geometry const&) = default;
  constexpr tile_geometry& operator=(tile_geometry&&) = default;

  /// Constructs a tile geometry from its bounding box
  constexpr tile_geometry(box_t bounding_box)
   : cell_length_(cell_length(bounding_box))
   , x_first_cell_(x_first_cell(bounding_box)) {}
};

}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
