#pragma once
/// \file
///
/// Square structured grid bounds
#include <hm3/geometry/dimensions.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace grid {
namespace structured {

namespace detail {

constexpr suint_t cells_per_length(suint_t nic, suint_t nhl) noexcept {
  return 2 * nhl + nic;
}
constexpr suint_t first_halo_left(suint_t /*nic*/, suint_t /* nhl*/) noexcept {
  return 0;
}
constexpr suint_t last_halo_left(suint_t /*nic*/, suint_t nhl) noexcept {
  return nhl - 1;
}
constexpr suint_t first_internal(suint_t /*nic*/, suint_t nhl) noexcept {
  return nhl;
}
constexpr suint_t last_internal(suint_t nic, suint_t nhl) noexcept {
  return nhl + nic - 1;
}
constexpr suint_t first_halo_right(suint_t nic, suint_t nhl) noexcept {
  return nhl + nic;
}
constexpr suint_t last_halo_right(suint_t nic, suint_t nhl) noexcept {
  return 2 * nhl + nic - 1;
}

}  // namespace detail

/// Bounds of a square structured grid
///
/// Nd: number of spatial dimensions
/// Nic: number of internal cells per dimension
/// Nhl: number of of halo layers
///
template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct bounds : geometry::dimensional<Nd> {
  /// \name Sizes
  ///@{

  /// Number of halo layers
  static constexpr suint_t halo_layers() noexcept { return Nhl; }
  /// Number of internal cells per length
  static constexpr suint_t internal_cells_per_length() noexcept { return Nic; }
  /// Number of cells per length
  static constexpr suint_t cells_per_length() noexcept {
    return detail::cells_per_length(Nic, Nhl);
  }
  /// Total number of cells
  static constexpr suint_t size() noexcept {
    return math::ipow(cells_per_length(), Nd);
  }
  /// Total number of internal cells
  static constexpr suint_t internal_cells() noexcept {
    return math::ipow(internal_cells_per_length(), Nd);
  }
  /// Total number of halo cells
  static constexpr suint_t halo_cells() noexcept {
    return size() - internal_cells();
  }

  ///@} // Sizes

  /// \name Coordinates
  ///@{

  /// 1D Coordinate of first halo cell of the tile with coordinates smaller than
  /// the internal cells
  static constexpr suint_t first_halo_left() noexcept {
    return detail::first_halo_left(Nic, Nhl);
  }
  /// 1D Coordinate of last halo cell of the tile with coordinates smaller than
  /// the internal cells
  static constexpr suint_t last_halo_left() noexcept {
    return detail::last_halo_left(Nic, Nhl);
  }
  /// 1D Coordinate of first internal cell
  static constexpr suint_t first_internal() noexcept {
    return detail::first_internal(Nic, Nhl);
  }
  /// 1D Coordinate of last internal cell
  static constexpr suint_t last_internal() noexcept {
    return detail::last_internal(Nic, Nhl);
  }
  /// 1D Coordinate of first internal cell (including \p h halo layers)
  static constexpr suint_t first_internal(suint_t h) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    return detail::first_internal(Nic, Nhl) - h;
  }
  /// 1D Coordinate of last internal cell (including \p h halo layers)
  static constexpr suint_t last_internal(suint_t h) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    return detail::last_internal(Nic, Nhl) + h;
  }
  /// 1D Coordinate of first halo cell of the tile with coordinates larger than
  /// the internal cells
  static constexpr suint_t first_halo_right() noexcept {
    return detail::first_halo_right(Nic, Nhl);
  }
  /// 1D Coordinate of last halo cell of the tile with coordinates larger than
  /// the internal cells
  static constexpr suint_t last_halo_right() noexcept {
    return detail::last_halo_right(Nic, Nhl);
  }
  static constexpr auto cpl = cells_per_length();
  static constexpr auto fhl = first_halo_left();
  static constexpr auto lhl = last_halo_left();
  static constexpr auto fi  = first_internal();
  static constexpr auto li  = last_internal();
  static constexpr auto fhr = first_halo_right();
  static constexpr auto lhr = last_halo_right();
};

}  // namespace structured
}  // namespace grid
}  // namespace hm3
