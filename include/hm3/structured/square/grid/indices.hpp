#pragma once
/// \file
///
/// Indices into a square structured grid
#include <hm3/structured/square/grid/halos.hpp>
#include <hm3/structured/square/tile.hpp>

namespace hm3 {
namespace structured {
namespace square {
namespace grid {

/// Square structured grid indices
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of cells per dimension
/// \tparam Nhl number of halo layers
template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct indices : bounds<Nd, Nic, Nhl>, tile<Nd, bounds<Nd, Nic, Nhl>::size()> {
  static_assert(Nhl > 0, "zero halo layers not supported");
  using bounds                  = bounds<Nd, Nic, Nhl>;
  using tile                    = tile::indices<Nd, Nic + 2 * Nhl>;
  using tile_coordinate         = typename tile::coordinate;
  using tile_index              = typename tile::index;
  using tile_indexed_coordinate = typename tile::indexed_coordinate;

  /// Sizes
  ///@{

  using bounds::first_internal;
  using bounds::last_internal;
  using bounds::cells_per_length;

  /// Total number of cells
  static constexpr auto size() noexcept {
    static_assert(bounds::size() == tile::size(), "");
    return tile::size();
  }

  ///@} // Sizes

  /// \name Coordinates
  ///@{

  /// Cell coordinates
  struct x : tile_indexed_coordinate {
    using tile_indexed_coordinate::tile_indexed_coordinate;
    using tile_indexed_coordinate::operator=;

    constexpr bool is_internal() const noexcept {
      for (suint_t d = 0; d < Nd; ++d) {
        const auto i = (*this)[d];
        if (i < first_internal() or i >= last_internal()) { return false; }
      }
      return true;
    }
    constexpr bool is_halo() const noexcept { return !is_internal(); }
  };

  /// Internal-halo cell pair coordinates
  struct x_ih {
    x internal;
    x halo;
  };

  ///@}  // Coordinates

  /// \name Internal iteration
  ///@{

  /// Iterates over all the internal cells in the grid
  ///
  /// Optional increase the size of the internal cells tile by \p h halo layers
  template <typename F>
  static constexpr for_each_internal(F&& f, suint_t h = 0) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    tile::for_each(x::constant(first_internal(h)),
                   x::constant(last_internal(h)),
                   [&f](auto&& i) { f(x(std::forward<decltype(i)>(i))); });
  }

  /// Iterates over all the halo cells in the grid
  ///
  /// \note To be used when performing grid-local operations
  template <typename F>  //
  static constexpr for_each_halo(F&& f) noexcept {
    auto g = [&f](auto&& i) { f(x(std::forward<decltype(i)>(i))); };
    tile::for_each_ring(x::constant(0), x::constant(cells_per_length()), g,
                        Nhc - 1);
  }

  template <typename F, typename Neighbor>
  static constexpr for_each_halo(Neighbor n, F&& f) {
    auto hb = halo_tile_bounds(n, bounds{});
  }

  ///@} Internal iteration

  /// TODO: Closest internal cell of a halo cell
  ///
  /// \note For each halo cell there is an unique internal cell that its the
  /// closest to it.
  ///
  /// This is usefull when implementing boundary conditions directly on halos
  /// (e.g. for boundaries aligned with the boundary internal cells surfaces
  /// like when simulating something on a cube)
};

}  // namespace grid
}  // namespace square
}  // namespace structured
}  // namespace hm3
