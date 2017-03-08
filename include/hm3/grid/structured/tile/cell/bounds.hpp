#pragma once
/// \file
///
/// Square tile cell bounds
#include <hm3/geometry/fwd.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/math/core.hpp>
#include <hm3/types.hpp>

namespace hm3::grid::structured::tile::cell {

/// Number of cells per length in the square tile
constexpr tidx_t length(tidx_t nc) { return nc; }

/// Total number of cells
constexpr tidx_t size(dim_t ad, tidx_t nc) {
  return math::ipow(nc, static_cast<tidx_t>(ad));
}

/// Cell bounds of a square tile
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <dim_t Ad, tidx_t Nc>
struct bounds : geometry::with_ambient_dimension<Ad> {
  /// Number of cells per length in the square tile.
  static constexpr tidx_t length() noexcept { return static_cast<tidx_t>(Nc); }
  /// Total number of cells in the tile.
  static constexpr tidx_t size() noexcept {
    return cell::size(static_cast<tidx_t>(Ad), Nc);
  }
};

}  // namespace hm3::grid::structured::tile::cell
