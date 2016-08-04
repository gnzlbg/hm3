#pragma once
/// \file
///
/// Square tile surface bounds
#include <hm3/geometry/dimension.hpp>
#include <hm3/grid/hierarchical/tree/relations/tree.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace surface {

/// Number of sides per direction
constexpr tidx_t sides_per_dir(dim_t /*nd*/, tidx_t nc) noexcept {
  return nc + 1;
}

/// Number of surfaces per tile side
constexpr tidx_t size_per_side(dim_t nd, tidx_t nc) noexcept {
  return math::ipow(nc, tidx_t(nd - 1));
}

/// Number of surfaces per direction
constexpr tidx_t size_per_dir(dim_t nd, tidx_t nc) noexcept {
  return size_per_side(nd, nc) * sides_per_dir(nd, nc);
}

/// Total number of surfaces in the tile
constexpr tidx_t size(dim_t nd, tidx_t nc) noexcept {
  return nd * size_per_dir(nd, nc);
}

constexpr tidx_t size_per_cell(dim_t nd, tidx_t /*nc*/) noexcept {
  return tree::no_faces(nd, nd - 1);
}

/// Surface bounds of a square tile
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <dim_t Nd, tidx_t Nc>  //
struct bounds : geometry::dimensional<Nd> {
  /// Nubmer of sides per direction
  static constexpr tidx_t sides_per_dir() noexcept {
    return surface::sides_per_dir(Nd, Nc);
  }
  /// Nubmer of surfaces per side
  static constexpr tidx_t size_per_side() noexcept {
    return surface::size_per_side(Nd, Nc);
  }
  /// Number of surfaces per direction
  static constexpr tidx_t size_per_dir() noexcept {
    return surface::size_per_dir(Nd, Nc);
  }
  /// Total number of surfaces in the tile
  static constexpr tidx_t size() noexcept { return surface::size(Nd, Nc); }

  /// Number of surfaces per tile cell
  static constexpr tidx_t size_per_cell() noexcept {
    return surface::size_per_cell(Nd, Nc);
  }
};

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
