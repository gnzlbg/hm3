#pragma once
/// \file
///
/// Square tile surface bounds
#include <hm3/geometry/fwd.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/math/core.hpp>
#include <hm3/types.hpp>

namespace hm3::grid::structured::tile::surface {

/// Number of sides per direction
constexpr tidx_t sides_per_dir(dim_t /*nd*/, tidx_t nc) noexcept {
  return nc + 1;
}

/// Number of surfaces per tile side
constexpr tidx_t size_per_side(dim_t ad, tidx_t nc) noexcept {
  return math::ipow(nc, tidx_t(ad - 1));
}

/// Number of surfaces per direction
constexpr tidx_t size_per_dir(dim_t ad, tidx_t nc) noexcept {
  return size_per_side(ad, nc) * sides_per_dir(ad, nc);
}

/// Total number of surfaces in the tile
constexpr tidx_t size(dim_t ad, tidx_t nc) noexcept {
  return ad * size_per_dir(ad, nc);
}

constexpr tidx_t size_per_cell(dim_t ad, tidx_t /*nc*/) noexcept {
  return tree::no_faces(ad, ad - 1);
}

/// Surface bounds of a square tile
///
/// \tparam Ad number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <dim_t Ad, tidx_t Nc>
struct bounds : geometry::with_ambient_dimension<Ad> {
  /// Nubmer of sides per direction
  static constexpr tidx_t sides_per_dir() noexcept {
    return surface::sides_per_dir(Ad, Nc);
  }
  /// Nubmer of surfaces per side
  static constexpr tidx_t size_per_side() noexcept {
    return surface::size_per_side(Ad, Nc);
  }
  /// Number of surfaces per direction
  static constexpr tidx_t size_per_dir() noexcept {
    return surface::size_per_dir(Ad, Nc);
  }
  /// Total number of surfaces in the tile
  static constexpr tidx_t size() noexcept { return surface::size(Ad, Nc); }

  /// Number of surfaces per tile cell
  static constexpr tidx_t size_per_cell() noexcept {
    return surface::size_per_cell(Ad, Nc);
  }
};

}  // namespace hm3::grid::structured::tile::surface
