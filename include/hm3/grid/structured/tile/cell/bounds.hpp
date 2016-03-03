#pragma once
/// \file
///
/// Square tile cell bounds
#include <hm3/geometry/dimensions.hpp>
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace tile {
namespace cell {

/// Number of cells per length in the square tile
constexpr index_type length(index_type nc) { return nc; }

/// Total number of cells
constexpr index_type size(index_type nd, index_type nc) {
  return math::ipow(nc, nd);
}

/// Cell bounds of a square tile
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <suint_t Nd, suint_t Nc>  //
struct bounds : geometry::dimensional<Nd> {
  /// Number of cells per length in the square tile.
  static constexpr index_type length() noexcept {
    return static_cast<index_type>(Nc);
  }
  /// Total number of cells in the tile.
  static constexpr index_type size() noexcept {
    return cell::size(static_cast<index_type>(Nd), static_cast<index_type>(Nc));
  }
};

}  // namespace cell
}  // namespace tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
