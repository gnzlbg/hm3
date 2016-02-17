#pragma once
/// \file
///
/// Square structured grid bounds
#include <hm3/geometry/dimensional.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace structured {
namespace square {

/// Bounds of a square structured grid
///
/// Nd: number of spatial dimensions
/// Nic: number of internal cells per dimension
/// Nhl: number of of halo layers
///
template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct bounds : geometry::dimensional<Nd> {
  using geometry::dimensional<Nd>::dimension;
  using geometry::dimensional<Nd>::dimensions;

  static_assert(Nd > 0 and Nd <= 3, "dimension out of bounds [1, 3]");
  static_assert(Nic > 0, "#of internal cells is 0. At least one required.");

  /// \name Sizes
  ///@{
  static constexpr suint_t halo_layers() noexcept { return Nhl; }
  static constexpr suint_t internal_cells_per_length() noexcept { return Nic; }
  static constexpr suint_t internal_cells() noexcept {
    return math::ipow(Nic, Nd);
  }
  static constexpr suint_t cells_per_length() noexcept {
    return internal_cells_per_length() + suint_t{2} * halo_layers();
  }
  static constexpr suint_t size() noexcept {
    return math::ipow(cells_per_length(), Nd);
  }
  static_assert(size() > 0, "zero-sized grid");
  ///@} // Sizes
};

}  // namespace square
}  // namespace structured
}  // namespace hm3
