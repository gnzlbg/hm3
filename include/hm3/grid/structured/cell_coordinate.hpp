#pragma once
/// \file
///
/// Square structured grid coordinate
#include <hm3/grid/structured/bounds.hpp>
#include <hm3/grid/structured/tile/indexed_coordinate.hpp>
namespace hm3 {
namespace grid {
namespace structured {

/// Square structured grid coordinate
///
/// \tparam Nd number of spatial dimensions.
/// \tparam Nic number of internal cells.
/// \tparam Nhl number of halo layers.
template <suint_t Nd, suint_t Nic, suint_t Nhl>
struct coordinate
 : tile::indexed_coordinate<Nd, bounds<Nd, Nic, Nhl>::cells_per_length()> {
  using bounds  = bounds<Nd, Nic, Nhl>;
  using tile_ic = tile::indexed_coordinate<Nd, bounds::cells_per_length()>;

  using value_t        = typename tile_ic::value_t;
  using signed_value_t = typename tile_ic::signed_value_t;
  using index          = typename tile_ic::index;
  using offset_t       = typename tile_ic::offset_t;
  using tile_ic::tile_ic;
  using tile_ic::operator=;

  constexpr coordinate(tile_ic x) : tile_ic(std::move(x)) {}
  explicit operator tile_ic() { return static_cast<tile_ic>(*this); }

  /// Is the coordinate an internal cell.
  constexpr bool is_internal() const noexcept {
    for (suint_t d = 0; d < Nd; ++d) {
      const auto i = (*this)[d];
      if (i < bounds::first_internal() or i > bounds::last_internal()) {
        return false;
      }
    }
    return true;
  }

  /// Is the coordinate a halo cell.
  constexpr bool is_halo() const noexcept { return !is_internal(); }

  /// Constructs a grid coordinate from a constant index.
  static constexpr coordinate constant(value_t i) noexcept {
    return coordinate(tile_ic::constant(i));
  }

  /// Construct a grid coordinate from a cell index.
  static constexpr coordinate from(index i) noexcept {
    return {tile_ic::from(i)};
  }

  constexpr coordinate offset(value_t d, signed_value_t o) const noexcept {
    return tile_ic::offset(d, o);
  }

  constexpr coordinate offset(offset_t o) const noexcept {
    return tile_ic::offset(o);
  }

  constexpr coordinate offset(signed_value_t o) const noexcept {
    return tile_ic::offset(o);
  }

  constexpr coordinate offset_if_valid(signed_value_t o) const noexcept {
    return tile_ic::offset_if_valid(o);
  }
};

}  // namespace structured
}  // namespace grid
}  // namespace hm3
