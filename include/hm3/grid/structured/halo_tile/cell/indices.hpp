
#pragma once
/// \file
///
/// Indices for iterating over a square halo tile cells'
#include <hm3/grid/structured/halo_tile/cell/bounds.hpp>
#include <hm3/grid/structured/halo_tile/cell/coordinate.hpp>
#include <hm3/grid/structured/tile/cell/indices.hpp>

namespace hm3 {
namespace grid {
namespace structured {
namespace halo_tile {
namespace cell {

/// Square structured halo tile cell indices
///
/// \tparam Nd  number of spatial dimensions
/// \tparam Nic number of internal (non-halo) cells per dimension
/// \tparam Nhl number of halo layers
template <dim_t Nd, tidx_t Nic, tidx_t Nhl>  //
struct indices : bounds<Nd, Nic, Nhl>,
                 tile::cell::indices<Nd, bounds<Nd, Nic, Nhl>::length()> {
  static_assert(Nhl > 0, "zero halo layers not supported");

  using bounds = bounds<Nd, Nic, Nhl>;

 private:  // members from tile, used to call tile methods
  using tile_cell_indices = tile::cell::indices<Nd, bounds::length()>;
  using tile_coordinate   = typename tile_cell_indices::coordinate;

 public:
  using index      = typename tile_cell_indices::index;
  using coordinate = coordinate<Nd, Nic, Nhl>;

  /// Sizes
  ///@{

  using bounds::first_internal;
  using bounds::last_internal;
  using bounds::length;
  using bounds::halo_layers;
  using bounds::size;
  static_assert(bounds::size() == tile_cell_indices::size(), "");

  ///@} // Sizes

  /// \name Internal iteration
  ///@{

  /// Iterates over all cells in the grid
  template <typename F> static constexpr void for_each(F&& f) noexcept {
    tile_cell_indices::for_each(
     [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); });
  }

  /// Iterates over all the internal cells in the grid
  ///
  /// Optional increase the size of the internal cells tile by \p h halo layers
  template <typename F>
  static constexpr void for_each_internal(F&& f, tidx_t h = 0) noexcept {
    HM3_ASSERT(h <= halo_layers(), "h {} is not <= halo layers {}", h,
               halo_layers());
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each(tile_coordinate::constant(first_internal(h)),
                                tile_coordinate::constant(last_internal(h)), g);
  }

  /// Iterates over all the halo cells in the grid
  ///
  /// \note To be used when performing grid-local operations
  template <typename F>  //
  static constexpr void for_each_halo(F&& f) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each_ring(
     tile_coordinate::constant(0),
     tile_coordinate::constant(bounds::last_halo_right()), g,
     halo_layers() - 1);
  }

  /// Iterates over the halos that overlap neighor's \p n internal cells:
  template <typename F, typename Neighbor>
  static constexpr void for_each_halo(Neighbor n, F&& f) {
    auto hb = halos(n, bounds{});
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each(tile_coordinate(hb.from),
                                tile_coordinate(hb.to), g);
  }

  template <typename F, typename Manifold>
  static constexpr void for_each_neighbor(coordinate x, Manifold m, F&& f,
                                          sint_t dist = 1) {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each_neighbor(tile_coordinate(x), m, g, dist);
  }

  template <typename F>
  static constexpr void for_each_neighbor(coordinate x, F&& f,
                                          sint_t dist = 1) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each_neighbor(tile_coordinate(x), g, dist);
  }

  template <typename F>
  static constexpr void for_each_ring(const coordinate from,
                                      const coordinate to, F&& f,
                                      tidx_t w = 0) noexcept {
    auto g
     = [&f](auto&& i) { return f(coordinate(std::forward<decltype(i)>(i))); };
    tile_cell_indices::for_each_ring(tile_coordinate(from), tile_coordinate(to),
                                     g, w);
  }
  ///@} Internal iteration

  template <typename P>
  static constexpr coordinate closest_cell(coordinate x, P&& p,
                                           tidx_t max_cell_distance
                                           = length()) noexcept {
    return tile_cell_indices::closest_cell(x, std::forward<P>(p),
                                           max_cell_distance);
  }

  /// External sub-tile range
  static auto sub_tile(coordinate from, coordinate to) noexcept {
    return tile_cell_indices::subtile_view(std::move(from), std::move(to))
           | view::transform([](auto&& i) {
               return coordinate(std::forward<decltype(i)>(i));
             });
  }

  static constexpr bool is_internal(coordinate i) noexcept {
    return i.is_internal();
  }

  static constexpr coordinate closest_internal_cell(coordinate x,
                                                    tidx_t max_cell_distance
                                                    = length()) noexcept {
    return closest_cell(x, [](coordinate y) { return is_internal(y); },
                        max_cell_distance);
  }
};

}  // namespace cell
}  // namespace halo_tile
}  // namespace structured
}  // namespace grid
}  // namespace hm3
