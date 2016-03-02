#pragma once
/// \file
///
/// Square tile bounds
#include <hm3/grid/structured/tile/index_type.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace structured {
namespace square {
namespace tile {

/// Square tile length
constexpr index_type length(index_type nc) { return nc; }

/// Square tile size
constexpr index_type size(index_type nd, index_type nc) {
  return math::ipow(nc, nd);
}

/// Square tile bounds
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nc number of cells per tile length (the tile is square)
template <suint_t Nd, suint_t Nc>  //
struct bounds {
  /// Square tile length
  static constexpr index_type length() noexcept {
    return static_cast<index_type>(Nc);
  }
  /// Square tile size
  static constexpr index_type size() noexcept {
    return tile::size(static_cast<index_type>(Nd), static_cast<index_type>(Nc));
  }
};

}  // namespace tile
}  // namespace square
}  // namespace structured
}  // namespace hm3
