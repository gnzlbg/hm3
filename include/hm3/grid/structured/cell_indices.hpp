#pragma once
/// \file
///
/// Indices into a square structured grid
#include <hm3/grid/structured/coordinate.hpp>
#include <hm3/grid/structured/halos.hpp>
#include <hm3/grid/structured/tile.hpp>

namespace hm3 {
namespace grid {
namespace structured {

/// Square structured grid indices
///
/// \tparam Nd number of spatial dimensions
/// \tparam Nic number of cells per dimension
/// \tparam Nhl number of halo layers
template <suint_t Nd, suint_t Nic, suint_t Nhl>  //
struct indices : bounds<Nd, Nic, Nhl>,
                 tile::indices<Nd, bounds<Nd, Nic, Nhl>::size()> {
  static_assert(Nhl > 0, "zero halo layers not supported");
  using bounds          = bounds<Nd, Nic, Nhl>;
  using index_type      = typename tile::index_type;
  using tile            = tile::indices<Nd, Nic + 2 * Nhl>;
  using index           = typename tile::index;
  using coordinate      = coordinate<Nd, Nic, Nhl>;
  using tile_coordinate = typename tile::coordinate;
  /// Sizes
  ///@{

  using bounds::first_internal;
  using bounds::last_internal;
  using bounds::cells_per_length;
  using bounds::halo_layers;
  using bounds::size;
  static_assert(bounds::size() == tile::size(), "");

  ///@} // Sizes

  /// \name Internal iteration
  ///@{

  /// Iterates over all cells in the grid
  template <typename F> static constexpr void for_each(F&& f) noexcept {
    tile::for_each(
     [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); });
  }

  /// Iterates over all the internal cells in the grid
  ///
  /// Optional increase the size of the internal cells tile by \p h halo layers
  template <typename F>
  static constexpr void for_each_internal(F&& f, suint_t h = 0) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each(tile_coordinate::constant(first_internal(h)),
                   tile_coordinate::constant(last_internal(h)), g);
  }

  /// Iterates over all the halo cells in the grid
  ///
  /// \note To be used when performing grid-local operations
  template <typename F>  //
  static constexpr void for_each_halo(F&& f) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each_ring(tile_coordinate::constant(0),
                        tile_coordinate::constant(bounds::last_halo_right()), g,
                        halo_layers() - 1);
  }

  /// Iterates over the halos that overlap neighor's \p n internal cells:
  template <typename F, typename Neighbor>
  static constexpr void for_each_halo(Neighbor n, F&& f) {
    auto hb = halo_tile_bounds(n, bounds{});
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each(hb.from, hb.to, g);
  }

  template <typename F, typename Manifold>
  static constexpr void for_each_neighbor(coordinate x, Manifold m, F&& f,
                                          sint_t dist = 1) {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each_neighbor(tile_coordinate(x), m, g, dist);
  }

  template <typename F>
  static constexpr void for_each_neighbor(coordinate x, F&& f,
                                          sint_t dist = 1) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each_neighbor(tile_coordinate(x), g, dist);
  }

  template <typename F>
  static constexpr void for_each_ring(const coordinate from,
                                      const coordinate to, F&& f,
                                      index_type w = 0) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile::for_each_ring(tile_coordinate(from), tile_coordinate(to), g, w);
  }
  ///@} Internal iteration

  template <typename P>
  static constexpr coordinate closest_cell(
   coordinate x, P&& p,
   index_type max_cell_distance = bounds::cells_per_length()) noexcept {
    return tile::closest_cell(x, std::forward<P>(p), max_cell_distance);
  }

  /// External sub-tile range
  static auto sub_tile(coordinate from, coordinate to) noexcept {
    return tile::subtile_view(std::move(from), std::move(to))
           | view::transform([](auto&& i) {
               return coordinate(std::forward<decltype(i)>(i));
             });
  }
};

}  // namespace structured
}  // namespace grid
}  // namespace hm3
