#pragma once
/// \file
///
/// Square structured halo tile coordinate
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/grid/structured/tile/cell/indexed_coordinate.hpp>
namespace hm3::grid::structured::halo_tile::cell {

/// Square structured grid cell coordinate
///
/// \tparam Ad  number of spatial dimensions.
/// \tparam Nic number of internal cells.
/// \tparam Nhl number of halo layers.
template <dim_t Ad, tidx_t Nic, tidx_t Nhl>
struct coordinate
 : tile::cell::indexed_coordinate<Ad, bounds<Ad, Nic, Nhl>::length()> {
  using self    = coordinate;
  using bounds  = bounds<Ad, Nic, Nhl>;
  using tile_ic = tile::cell::indexed_coordinate<Ad, bounds::length()>;

  using value_t        = typename tile_ic::value_t;
  using signed_value_t = typename tile_ic::signed_value_t;
  using index          = typename tile_ic::index;
  using offset_t       = typename tile_ic::offset_t;
  using tile_ic::tile_ic;
  using tile_ic::operator=;

  constexpr coordinate()                  = default;
  constexpr coordinate(coordinate const&) = default;
  constexpr coordinate(coordinate&&)      = default;
  constexpr coordinate& operator=(coordinate const&) = default;
  constexpr coordinate& operator=(coordinate&&) = default;

  constexpr coordinate(tile_ic x) : tile_ic(std::move(x)) {}

  // constexpr operator tile_ic() const noexcept {
  //   return tile_ic(static_cast<tile_ic const&>(*this));
  // }

  /// Is the coordinate an internal cell.
  constexpr bool is_internal() const noexcept {
    for (dim_t d = 0; d < Ad; ++d) {
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
  static constexpr self constant(value_t i) noexcept {
    return self(tile_ic::constant(i));
  }

  /// Construct a grid coordinate from a cell index.
  static constexpr self from(index i) noexcept { return {tile_ic::from(i)}; }

  constexpr self offset(value_t d, signed_value_t o) const noexcept {
    return tile_ic::offset(d, o);
  }

  constexpr self offset(offset_t o) const noexcept {
    return tile_ic::offset(o);
  }

  constexpr self offset(signed_value_t o) const noexcept {
    return tile_ic::offset(o);
  }

  constexpr self offset_if_valid(signed_value_t o) const noexcept {
    return tile_ic::offset_if_valid(o);
  }
};

}  // namespace hm3::grid::structured::halo_tile::cell

namespace std {

template <hm3::dim_t Ad, hm3::grid::structured::tile::tidx_t Nc,
          hm3::grid::structured::tile::tidx_t Nhl>
struct hash<hm3::grid::structured::halo_tile::cell::coordinate<Ad, Nc, Nhl>> {
  constexpr std::size_t operator()(
   hm3::grid::structured::halo_tile::cell::coordinate<Ad, Nc, Nhl> const& c)
   const noexcept {
    return static_cast<std::size_t>(*(c.idx_));
  }
};

}  // namespace std
