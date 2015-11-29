#pragma once
/// \file
///
/// Hierarchical Cartesian Block Structured Solver grids
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/state/grid.hpp>
#include <hm3/solver/state/neighbors.hpp>

namespace hm3 {
namespace solver {

template <typename Block>
struct hierarchical_block_structured_grid : dimensional<Block::dimension()> {
  using block_t = Block;
  using dimensional<Block::dimension()>::dimension;
  using block_grid_t = solver::state::grid<dimension()>;
  using grid_t       = typename block_grid_t::tree_t;
  using grid_neighbors_t
   = solver::state::neighbors<tree::max_no_neighbors(dimension()) + 1>;
  using blocks_t  = std::vector<block_t>;
  using block_idx = grid_node_idx;
  block_grid_t grid;
  grid_neighbors_t neighbors;
  blocks_t blocks_;

  hierarchical_block_structured_grid(grid_t& g, grid_idx gidx,
                                     block_idx block_capacity)
   : grid(g, gidx, block_capacity)
   , neighbors(block_capacity)
   , blocks_(*block_capacity) {}

  grid_idx idx() const noexcept { return grid.idx(); }

  block_t& block(block_idx bidx) noexcept { return blocks_[*bidx]; }

  block_t const& block(block_idx bidx) const noexcept { return blocks_[*bidx]; }

  auto blocks() const noexcept {
    return grid() | view::transform(
                      [&](auto&& idx) -> block_t const& { return block(idx); });
  }

  auto blocks() noexcept {
    return grid() | view::transform(
                      [&](auto&& idx) -> block_t& { return block(idx); });
  }

  block_idx push(tree_node_idx n) noexcept {
    block_idx bidx = grid.push(n);
    blocks_[*bidx].reinitialize(grid.level(bidx), grid.tree().geometry(n));
    neighbors.push(bidx, grid.neighbors(bidx));
    return bidx;
  }
  void pop(block_idx bidx) {
    grid.pop(bidx);
    neighbors.pop(bidx);
    blocks_[*bidx].clear();
  }
  /// Projection: project parent to children
  template <typename Projection>
  void refine(block_idx bidx, Projection&& projection) {
    auto guard = grid.refine(bidx);
    projection(bidx, guard());
    neighbors.pop(bidx);  // remove parent first
    for (auto cb : guard()) { neighbors.push(cb, grid.neighbors(cb)); }
  }

  /// Restriction: restrict children to parents
  template <typename Restriction>
  void coarsen(block_idx bidx, Restriction&& restriction) {
    auto guard = grid.coarsen(bidx);
    restriction(bidx, guard());
    for (auto cb : guard()) {
      blocks_[*cb].clear();
      neighbors.pop(cb);
    }
    neighbors.push(guard.parent);
  }

  auto no_cells() const noexcept { return block_t::size() * (*grid.size()); }

  auto all_cells() const noexcept {
    return boxed_ints<cell_idx>(0, no_cells());
  }

  bool is_internal(cell_idx c) const noexcept {
    auto b   = block(c);
    auto bci = block_cell_idx(c);
    return block(b).is_internal(bci);
  }

  bool is_in_block(cell_idx c, block_idx b) const noexcept {
    return block(c) == b;
  }

  static block_idx block(cell_idx c) noexcept {
    return block_idx{*c / block_t::size()};
  }

  static uint_t block_cell_idx(cell_idx c) noexcept {
    return (*c) - (*block(c)) * block_t::size();
  }

  auto cells() const noexcept {
    return all_cells()
           | view::filter([&](cell_idx c) { return is_internal(c); });
  }

  auto all_cells(block_idx b) const noexcept {
    return all_cells()
           | view::filter([&, b](cell_idx c) { return is_in_block(c, b); });
  }

  auto geometry(cell_idx c) const noexcept {
    return block(block(c)).geometry(block_cell_idx(c));
  }
};

}  // namespace solver
}  // namespace hm3
