#pragma once
/// \file
///
/// Square tile surface bounds
#include <hm3/geometry/dimensions.hpp>
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
constexpr index_type sides_per_dir(index_type /*nd*/, index_type nc) noexcept {
  return nc + 1;
}

/// Number of surfaces per tile side
constexpr index_type size_per_side(index_type nd, index_type nc) noexcept {
  return math::ipow(nc, nd - 1);
}

/// Number of surfaces per direction
constexpr index_type size_per_dir(index_type nd, index_type nc) noexcept {
  return size_per_side(nd, nc) * sides_per_dir(nd, nc);
}

/// Total number of surfaces in the tile
constexpr index_type size(index_type nd, index_type nc) noexcept {
  return nd * size_per_dir(nd, nc);
}

constexpr index_type size_per_cell(index_type nd, index_type /*nc*/) noexcept {
  return tree::no_faces(nd, nd - 1);
}

/// Surface bounds of a square tile
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <suint_t Nd, suint_t Nc>  //
struct bounds : geometry::dimensional<Nd> {
  /// Nubmer of sides per direction
  static constexpr index_type sides_per_dir() noexcept {
    return surface::sides_per_dir(static_cast<index_type>(Nd),
                                  static_cast<index_type>(Nc));
  }
  /// Nubmer of surfaces per side
  static constexpr index_type size_per_side() noexcept {
    return surface::size_per_side(static_cast<index_type>(Nd),
                                  static_cast<index_type>(Nc));
  }
  /// Number of surfaces per direction
  static constexpr index_type size_per_dir() noexcept {
    return surface::size_per_dir(static_cast<index_type>(Nd),
                                 static_cast<index_type>(Nc));
  }
  /// Total number of surfaces in the tile
  static constexpr index_type size() noexcept {
    return surface::size(static_cast<index_type>(Nd),
                         static_cast<index_type>(Nc));
  }

  /// Number of surfaces per tile cell
  static constexpr index_type size_per_cell() noexcept {
    return surface::size_per_cell(static_cast<index_type>(Nd),
                                  static_cast<index_type>(Nc));
  }
};

}  // namespace surface
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
