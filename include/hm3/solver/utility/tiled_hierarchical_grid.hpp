#pragma once
/// \file
///
/// Tiled hierarchical Cartesian solver grid
///
/// \todo Rename file
/// \todo Remove unstable APIs
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/grid/hierarchical/client/multi.hpp>
#include <hm3/solver/geometry/types.hpp>
#include <hm3/solver/types.hpp>

namespace hm3::solver {

/// Hierarchical grid of tiles
///
/// \tparam Tile Type of the tile to store per octree node
template <typename Tile>
struct tiled_hierarchical_grid : hg::with_ambient_dimension<ad_v<Tile>> {
  using hg::with_ambient_dimension<ad_v<Tile>>::ambient_dimension;

  /// Type of the tile
  using tile_t     = Tile;
  using tile_idx_t = grid_node_idx;  ///< Index of a tile within the grid
  using tile_local_cell_idx_t = typename tile_t::cell_coordinate_t;

  /// Grid client
  using grid_client_t = grid::hierarchical::client::multi<ambient_dimension()>;

  /// Grid type
  using grid_t = typename grid_client_t::tree_t;

  /// Tile storage
  using tiles_t = vector<tile_t>;

  /// \name Data
  ///@{

  grid_client_t grid_client;  ///< Client to the stored grid
  tiles_t tiles_;             ///< Storage of the tile

  ///@} // Data

  tiled_hierarchical_grid(grid_t& g, grid_idx gidx, tile_idx_t tile_capacity)
   : grid_client(g, gidx, tile_capacity), tiles_(*tile_capacity) {}

  /// Grid index
  grid_idx idx() const noexcept { return grid_client.idx(); }

  auto grid() const noexcept { return grid_client(); }
  auto const& tree() const noexcept { return grid_client.tree(); }

  /// Returns a reference to tile at index \p t
  tile_t& tile(tile_idx_t t) noexcept { return tiles_[*t]; }
  /// Returns a reference to tile at index \p t
  tile_t const& tile(tile_idx_t t) const noexcept { return tiles_[*t]; }

  /// Range of tiles
  auto tiles() const noexcept {
    return grid_client() | view::transform([&](auto&& idx) -> tile_t const& {
             return tile(idx);
           });
  }

  /// Range of tiles
  auto tiles() noexcept {
    return grid_client()
           | view::transform([&](auto&& idx) -> tile_t& { return tile(idx); });
  }

  /// Append a new tile at the tree node \p n
  ///
  /// \pre The tree node cannot contain a tile that is part of the current grid
  tile_idx_t push(tree_node_idx n) noexcept {
    HM3_ASSERT(!grid_client.in_tree(n), "node {} is already in grid {}", n,
               idx());
    tile_idx_t t = grid_client.push(n);
    tiles_[*t].reinitialize(grid_client.level(t),
                            grid_client.tree().geometry(n));
    // HM3_ASSERT(!grid_client.in_tree(t), "push failed: tile {} is not in grid
    // {}", t,
    //            idx());
    return t;
  }

  auto neighbors(tile_idx_t t) const noexcept {
    //    return grid_client.cached_neighbors(t);
    return grid_client.grid_node_neighbors_in_tree(t);
  }

  /// Removes tile \p t from the grid
  void pop(tile_idx_t t) {
    // HM3_ASSERT(grid_client.in_tree(t), "node {} is not in grid {}", t,
    // idx());
    grid_client.pop(t);
    tiles_[*t].clear();
    // HM3_ASSERT(!grid_client.in_tree(t), "pop failed: node {} is still in grid
    // {}",
    // t,
    //            idx());
  }

  /// Projection: project parent tile \p t to children tiles
  template <typename Projection>
  void refine(tile_idx_t t, Projection&& projection) {
    grid_client.refine(t, [&](auto&& old_parent, auto&& new_children) {
      // TODO: only in debug? Or assert that they are cleared!
      for (auto nc : new_children) { tiles_[*nc].clear(); }
      projection(old_parent, new_children);
    });
  }

  /// Restriction: restrict siblings of tile \p t to its parent tile
  template <typename Restriction>
  void coarsen(tile_idx_t t, Restriction&& restriction) {
    grid_client.coarsen(t, [&](auto&& new_parent, auto&& old_children) {
      restriction(new_parent, old_children);
      for (auto oc : old_children) { tiles_[*oc].clear(); }
    });
  }

  /// \todo rename to size()
  /// \todo store the size here
  /// \todo add support for non-constant sized tiles (need to dispatch here)
  cell_idx_t no_cells() const noexcept {
    return tile_t::cells().size() * (*grid_client.size());
  }

  /// \name UNSTABLE API
  ///@{

  /// Range of all cells
  ///
  /// This encourages going over all cells in linear order instead of
  /// block wise, such that the index recomputation is slow and will be O(N)
  /// if tiles with run-time size are allowed.
  ///
  /// \todo unstable API
  ///
  auto all_cells() const noexcept {
    return boxed_ints<cell_idx_t>(cell_idx_t(0), no_cells());
  }

  bool is_internal(cell_idx_t c) const noexcept {
    auto tidx   = tile_idx(c);
    auto tlcidx = tile_local_cell_idx(c);
    return tile(tidx).cells().is_internal(tlcidx);
  }

  static constexpr bool is_in_tile(cell_idx_t c, tile_idx_t t) noexcept {
    return tile_idx(c) == t;
  }

  /// Tile containing the cell at index \p c
  static constexpr tile_idx_t tile_idx(cell_idx_t c) noexcept {
    return tile_idx_t{*c / tile_t::cells().size()};
  }

  /// Returns a reference to tile at index \p t
  tile_t& tile(cell_idx_t c) noexcept { return tile(tile_idx(c)); }
  /// Returns a reference to tile at index \p t
  tile_t const& tile(cell_idx_t c) const noexcept { return tile(tile_idx(c)); }

  static tile_local_cell_idx_t tile_local_cell_idx(cell_idx_t c) noexcept {
    // TODO: this is only going to work fast for a constant number of cells per
    // tile
    return tile_local_cell_idx_t{(*c)
                                 - (*tile_idx(c)) * tile_t::cells().size()};
  }

  auto cells() const noexcept {
    return all_cells()
           | view::filter([&](cell_idx_t c) { return is_internal(c); });
  }

  auto all_cells(tile_idx_t t) const noexcept {
    return all_cells()
           | view::filter([&, t](cell_idx_t c) { return is_in_tile(c, t); });
  }

  auto geometry(cell_idx_t c) const noexcept {
    return tile(tile_idx(c)).geometry()(tile_local_cell_idx(c));
  }

  ///@}  UNSTABLE API
};

}  // namespace hm3::solver
