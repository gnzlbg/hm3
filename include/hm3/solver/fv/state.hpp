#pragma once
/// \file
///
/// Finite-volume solver state
#include <hm3/solver/fv/block.hpp>
#include <hm3/solver/state/grid.hpp>
#include <hm3/solver/state/neighbors.hpp>
// #include <hm3/solver/state/runge_kutta.hpp>
// #include <hm3/solver/state/boundary_conditions.hpp>

namespace hm3 {
namespace solver {
namespace fv {

/// Finite volume discretization state
template <typename Physics>  //
struct state : dimensional<Physics::dimension()> {
  using dimensional<Physics::dimension()>::dimension;
  using dimensional<Physics::dimension()>::dimensions;

  using block_idx = grid_node_idx;

  static constexpr uint_t nvars() noexcept { return Physics::nvars(); }

  static constexpr auto variables() noexcept { return view::iota(0, nvars()); }

  using block_grid_t = solver::state::grid<dimension()>;
  using grid_t       = typename block_grid_t::tree_t;
  using grid_neighbors_t
   = solver::state::neighbors<tree::max_no_neighbors(dimension()) + 1>;
  using block_t  = block<dimension(), nvars(), 100, 2>;
  using blocks_t = std::vector<block_t>;

  Physics physics;
  block_grid_t block_grid;
  grid_neighbors_t block_neighbors;
  blocks_t blocks_;

  state(grid_t& g, grid_idx gidx, block_idx block_capacity, Physics p)
   : physics(p)
   , block_grid(g, gidx, block_capacity)
   , block_neighbors(block_capacity)
   , blocks_(*block_capacity) {}

  grid_idx idx() const noexcept { return block_grid.idx(); }

  block_t& block(block_idx bidx) noexcept { return blocks_[*bidx]; }

  block_t const& block(block_idx bidx) const noexcept { return blocks_[*bidx]; }

  auto blocks() const noexcept {
    return block_grid() | view::transform([&](auto&& idx) -> block_t const& {
             return block(idx);
           });
  }

  auto blocks() noexcept {
    return block_grid() | view::transform(
                           [&](auto&& idx) -> block_t& { return block(idx); });
  }

  block_idx push(tree_node_idx n) noexcept {
    block_idx bidx = block_grid.push(n);
    blocks_[*bidx]
     = block_t(block_grid.level(bidx), block_grid.tree().geometry(n));
    block_neighbors.push(bidx, block_grid.neighbors(bidx));
    return bidx;
  }
  void pop(block_idx bidx) {
    block_grid.pop(bidx);
    block_neighbors.pop(bidx);
    blocks_[*bidx] = block_t{};
  }
  void refine(block_idx bidx) {
    auto guard = block_grid.refine(bidx);
    project_parent_to_children(bidx, guard());
    block_neighbors.pop(bidx);  // remove parent first
    for (auto cb : guard()) {
      block_neighbors.push(cb, block_grid.neighbors(cb));
    }
  }
  void coarsen(block_idx bidx) {
    auto guard = block_grid.coarsen(bidx);
    restrict_children_to_parent(bidx, guard());
    for (auto cb : guard()) {
      blocks_[*cb] = block_t{};
      block_neighbors.pop(cb);
    }
    block_neighbors.push(guard.parent);
  }
  //  void sort();

  auto no_cells() const noexcept {
    return block_t::size() * (*block_grid.size());
  }

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
    // std::cerr << "c: " << c << " bidx: " << block(c)
    //           << " bsize: " << block_t::size() << std::endl;
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

  decltype(auto) variables(cell_idx c) const noexcept {
    return (block(block(c))).variables(block_cell_idx(c));
  }
};

template <typename Physics>
bool operator==(state<Physics> const&, state<Physics> const&) noexcept {
  return false;
}

template <typename Physics>
bool operator!=(state<Physics> const& a, state<Physics> const& b) noexcept {
  return !(a == b);
}

}  // namespace fv
}  // namespace name
}  // namespace hm3
