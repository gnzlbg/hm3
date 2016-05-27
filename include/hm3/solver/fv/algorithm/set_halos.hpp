#pragma once
/// \file
///
/// Exchange halos
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Sets the tile halo cell values from the internal cell values
///
/// For a given tile, this function sets the halo cell values from the internal
/// cell values of its neighbors. If the neighobr tile:
///
/// 1. is at the same level as the tile, the internal cell values of the
/// neighbor are copied into the halo cell of the tile,
///
/// 2. is at a level _higher_ than the tile (i.e. its cells are smaller), the
/// internal cell values from the neighbor are _restricted_ to the halo cell
/// values of the tile,
//
/// 3. is at a level _lower_ than the tile (i.e. its cells are larger), the
/// internal cell values of the neighbor are _projected_ into the halo cells of
/// the tile.
///
/// Note: for properly applying a projection the gradients in the neighbor cells
/// need to be known. Computing this gradients requires the halo cell values of
/// the neighbor to be set. As a consequence the halo cells are set in two
/// steps:
///
/// 1. First all copies and restrictions are applied,
/// 2. Then the projections are applied
///
/// TODO:
/// 2. Apply projections from the root to the leafs by:
///    2.1 Computing the gradients of tiles at level l
///    2.2 Performing projections from level l to level l + 1
///    2.3 Advancing to level l + 1 until we are done.
/// (this might not be necessary since 2:1 makes it impossible to have
/// data dependencies between projection applications)
///
/// TODO: right now for a given tile and one of his neighbors, it is probed if
/// all the halo cells of the tile lie within the neighbor. There is full suport
/// in tile for iterating over sub-tiles, and halo_tile has support for halo
/// tiles between tiles "at the same level". This support should be increased
/// for iterating over halo tiles across levels and used here.
struct set_halos_fn {
  template <typename State, typename TIdx, typename Tile>
  void copy_internal_cells(State& s, TIdx tile_idx, Tile& tile,
                           TIdx neighbor_tile_idx, Tile& neighbor_tile) const
   noexcept {
    tile.cells().for_each_halo([&](auto tile_halo_cell) {
      auto x_h = tile.geometry().cell_centroid(tile_halo_cell);
      auto neighbor_tile_internal_cell
       = neighbor_tile.geometry().internal_cell_containing(x_h);
      if (!neighbor_tile_internal_cell) { return; }
      s.time_integration.lhs(tile)(tile_halo_cell)
       = s.time_integration.lhs(neighbor_tile)(neighbor_tile_internal_cell);
    });
  }

  template <typename State, typename TIdx, typename Tile>
  void restrict_internal_cells(State& s, TIdx tile_idx, Tile& tile,
                               TIdx neighbor_tile_idx,
                               Tile& neighbor_tile) const noexcept {
    tile.cells().for_each_halo([&](auto tile_halo_cell) {
      const auto x_h = tile.geometry().cell_centroid(tile_halo_cell);
      auto neighbor_tile_internal_cell
       = neighbor_tile.geometry().internal_cell_containing(x_h);
      if (!neighbor_tile_internal_cell) { return; }
      // zero halo:
      auto t_lhs = s.time_integration.lhs(tile)(tile_halo_cell);
      RANGES_FOR (auto&& v, s.variables()) { t_lhs(v) = 0.; }
      // average children values:
      const auto l_h           = tile.geometry().cell_length();
      static constexpr auto nd = State::dimension();
      for (auto child_pos : s.grid_client.tree().child_positions()) {
        auto x_child = child_centroid(child_pos, x_h, l_h);
        auto c = neighbor_tile.geometry().internal_cell_containing(x_child);
        HM3_ASSERT(c, "");  // all child must lie within the neighbors and they
                            // must be different cells
        t_lhs += s.time_integration.lhs(neighbor_tile)(c);
      }
      t_lhs /= s.grid_client.tree().no_children();
    });
  }

  template <typename State, typename TIdx, typename Tile, typename Lim>
  void project_internal_cells(State& s, TIdx tile_idx, Tile& tile,
                              TIdx neighbor_tile_idx, Tile& neighbor_tile,
                              Lim&& lim) const noexcept {
    tile.cells().for_each_halo([&](auto tile_halo_cell) {
      auto x_hc = tile.geometry().cell_centroid(tile_halo_cell);
      auto neighbor_tile_internal_cell
       = neighbor_tile.geometry().internal_cell_containing(x_hc);
      if (!neighbor_tile_internal_cell) { return; }  // TODO: use halo sub-tiles
      auto x_nc
       = neighbor_tile.geometry().cell_centroid(neighbor_tile_internal_cell);
      auto dx    = x_hc() - x_nc();
      auto t_lhs = s.time_integration.lhs(tile)(tile_halo_cell);
      auto nt_lhs
       = s.time_integration.lhs(neighbor_tile)(neighbor_tile_internal_cell);
      RANGES_FOR (auto&& d, s.dimensions()) {
        auto neighbor_gradients = compute_structured_gradient(
         s, neighbor_tile, neighbor_tile_internal_cell, d, lim);
        neighbor_gradients *= dx(d);
        neighbor_gradients += nt_lhs;
        t_lhs = neighbor_gradients;
      }
    });
  }

  /// Set halo cell values of all tiles
  template <typename State, typename Limiter>  //
  void operator()(State& s, Limiter&& lim) const noexcept {
    // 1. Copy internal cells into equal level halo cells and restrict from
    // finer to coarser levels
    for (auto&& tile_idx : s.grid()) {
      auto&& tile = s.tile(tile_idx);
      for (auto neighbor_tile_idx : s.neighbors(tile_idx)) {
        auto& neighbor_tile = s.tile(neighbor_tile_idx);
        if (tile.level == neighbor_tile.level) {  // copy
          copy_internal_cells(s, tile_idx, tile, neighbor_tile_idx,
                              neighbor_tile);
        } else if (tile.level < neighbor_tile.level) {  // restrict
          restrict_internal_cells(s, tile_idx, tile, neighbor_tile_idx,
                                  neighbor_tile);
        }
      }
    }

    // 2. Project from coarser cells into finer cells
    for (auto&& tile_idx : s.grid()) {
      auto&& tile = s.tile(tile_idx);
      for (auto neighbor_tile_idx : s.neighbors(tile_idx)) {
        auto& neighbor_tile = s.tile(neighbor_tile_idx);
        if (!(tile.level < neighbor_tile.level)) { continue; }
        project_internal_cells(s, tile_idx, tile, neighbor_tile_idx,
                               neighbor_tile, lim);
      }
    }
  }
};

namespace {
static constexpr auto&& set_halos = static_const<set_halos_fn>::value;
}  // namespace
/*
/// Set halo cells values of each tile
template <typename State, typename Limiter>
void set_halos(State& s, Limiter&& lim) {
// auto lim = limiter::van_albada;
// Iterate over all tiles and perform copies and restrictions (which are cell
// local)
for (auto&& bidx : s.grid()) {
  auto&& b = s.tile(bidx);
  for (auto bn_idx : s.neighbors(bidx)) {
    auto&& bn    = s.tile(bn_idx);
    auto&& b_lhs = s.time_integration.lhs(b);
    // If the level is the same: copy neighbor cells into tile halos:
    if (b.tile_level == bn.tile_level) {
      b.cells().for_each_halo([&](auto hc) {
        auto x_hc = b.geometry().cell_centroid(hc);
        // if (!bn.geometry().contains(x_hc)) { return; }
        auto nc = bn.geometry().internal_cell_containing(x_hc);
        // std::cerr << "x_hc: " << x_hc << " | nc: " << nc << std::endl;
        if (!nc) { return; }  // TODO: use halo sub-tiles

        b_lhs(hc) = s.time_integration.lhs(bn)(nc);
      });
    }

    // if this tile level is higher, project from neighbor cells into
    // this tile halos
    //
    // TODO: see below, this should be performed later
    if (b.tile_level > bn.tile_level) {
      b.cells().for_each_halo([&](auto hc) {
        auto x_hc = b.geometry().cell_centroid(hc);
        // if (!bn.in_grid(x_hc)) { return; }
        auto nc = bn.geometry().internal_cell_containing(x_hc);
        if (!nc) { return; }  // TODO: use halo sub-tiles
        b_lhs(hc) = s.time_integration.lhs(bn)(nc);
      });
    }

    // if this tile level is smaller, restrict from neighbor cells into this
    // tile halos
    if (b.tile_level < bn.tile_level) {
      // zero the variables
      b.cells().for_each_halo([&](auto hc) {
        auto x_hc = b.geometry().cell_centroid(hc);
        // if (!bn.in_grid(x_hc)) { return; }
        auto nc = bn.geometry().internal_cell_containing(x_hc);
        if (!nc) { return; }  // TODO: use halo sub-tiles
        RANGES_FOR (auto&& v, s.variables()) { b_lhs(hc)(v) = 0.; }
      });
      // zero the variables
      b.cells().for_each_halo([&](auto hc) {
        auto x_hc = b.geometry().cell_centroid(hc);
        // if (!bn.in_grid(x_hc)) { return; }
        auto nc = bn.geometry().internal_cell_containing(x_hc);
        if (!nc) { return; }  // TODO: use halo sub-tiles
        b_lhs(hc) += s.time_integration.lhs(bn)(nc);
      });
    }
  }
}

for (auto&& bidx : s.grid()) {
  auto&& b = s.tile(bidx);
  for (auto bn_idx : s.neighbors(bidx)) {
    auto&& bn    = s.tile(bn_idx);
    auto&& b_lhs = s.time_integration.lhs(b);

    if (b.tile_level > bn.tile_level) {
      b.cells().for_each_halo([&](auto hc) {
        auto x_hc = b.geometry().cell_centroid(hc);
        // if (!bn.in_grid(x_hc)) { return; }
        // auto nc   = bn.at_nh(x_hc);
        auto nc = bn.geometry().internal_cell_containing(x_hc);
        if (!nc) { return; }  // TODO: use halo sub-tiles
        auto x_nc = bn.geometry().cell_centroid(nc);
        auto dx   = x_hc() - x_nc();

        RANGES_FOR (auto&& d, s.dimensions()) {
          auto gradients = compute_structured_gradient(s, bn, nc, d, lim);
          gradients *= dx(d);
          gradients += b_lhs(hc);
          b_lhs(hc) = gradients;
        }
      });
    }
  }
}

// /// Iterate over all tiles and perform projects (which requires
// gradients
// /// on
// /// the parent cell, which can only be computed locally if the previous
// step
// /// was completed and the tiles and lower levels have already been
// /// projected)
// for (auto l : s.levels()) {
//   for (auto&& bidx : s.tile_ids()) {
//     if (s.tile(bidx).tile_level != l) { continue; }
//     auto&& b = s.tile(bidx);
//     for (auto bn_idx : s.neighbors(bidx)) {
//       auto&& bn = s.tile(bn_idx);
//       // If this tiles level is higher, project from neighbor cells
//       into
//       // tile halos
//       if (b.tile_level > bn.tile_level) {
//         n.for_each_halo([&](auto hc) {
//           auto x_hc + b.geometry().cell_centroid(hc);
//           auto nc = bn.at(x_hc);
//           if (!nc) { return; }
//           fv::compute_gradients(s, bn, nc);  // gradients of neighbor
//           fv::project(bn, nc, b, hc);        // from neighbor to halo
//         });
//       }
//     }
//   }
// }
}
*/
}  // namespace fv
}  // namespace solver
}  // namespace hm3
