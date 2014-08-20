#pragma once

/*
#include <array>
#include <vector>
#include <hm3/core/arithmetic.hpp>
#include <hm3/core/matrix.hpp>
#include <hm3/core/types.hpp>
#include <hm3/core/returns.hpp>
#include <hm3/io/properties.hpp>
#include <hm3/solver/fv/traits.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <class Tree, SInd nvars_, SInd idx_> struct state {
  using node_idx = node_idx_t<Tree>;
  using cell_idx = cell_idx_t<Tree>;
  using grid_idx = grid_idx_t<Tree>;

  struct surface_idx_tag {};
  using surface_idx = arithmetic<Ind, surface_idx_tag>;

  static constexpr SInd nd() { return Tree::nd; }
  static constexpr SInd nvars() { return nvars_; }
  static constexpr grid_idx idx() { return grid_idx{idx_}; }

  template <class T, TInt NoCols>
  using cell_matrix = dense::matrix<T, dense::dynamic, NoCols, cell_idx>;

  template <class T> using cell_vector = cell_matrix<T, 1>;

  /// properties:
  io::properties properties_;

  /// bounds:
  Ind max_no_cells_;
  Ind no_cells_;
  Ind max_no_surfaces_;
  Ind no_surfaces_;

  /// cell data:
  cell_vector<node_idx> node_ids_;
  cell_matrix<cell_idx, 2 * nd()> direct_neighbors_;
  cell_matrix<Num, nvars()> cell_variables_;
  cell_matrix<Num, nvars()> cell_rhs_;
  cell_matrix<Num, nd() * nvars()> cell_gradients_;
  cell_vector<Num> cell_volume_;
  cell_matrix<Num, nd()> cell_center_;
  cell_matrix<surface_idx, nd()> cell_surfaces_;

  /// surface data
  std::vector<std::array<Num, nd()>> surface_center_;
  std::vector<std::array<Num, nvars()>> surface_variables_;
  std::vector<std::array<Num, nd() * nvars()>> surface_gradients_;
  std::vector<std::array<cell_idx, 2>> surface_neighbors_;
  std::vector<std::array<Num, nvars()>> surface_flux_;
  std::vector<std::vector<bool>> is_boundary_surface_;

  explicit state(io::properties ps)
   : properties_(ps)
   , max_no_cells_(io::read<Ind>(ps, "max_no_cells"))
   , no_cells_(0)
   , max_no_surfaces_(nd() * max_no_cells_ / 2.)
   , no_surfaces_(0)
   // cell data:
   , node_ids_(max_no_cells_)
   , direct_neighbors_(max_no_cells_, 2 * nd())
   , cell_variables_(max_no_cells_, nvars())
   , cell_rhs_(max_no_cells_, nvars())
   , cell_gradients_(max_no_cells_, nd() * nvars())
   , cell_volume_(max_no_cells_)
   , cell_center_(max_no_cells_, nd())
   , cell_surfaces_(max_no_cells_, nd())
   // surface data:
   , surface_center_(max_no_surfaces_)
   , surface_variables_(max_no_surfaces_)
   , surface_gradients_(max_no_surfaces_)
   , surface_neighbors_(max_no_surfaces_)
   , surface_flux_(max_no_surfaces_)
   , is_boundary_surface_(max_no_surfaces_) {}
};

template <SInd idx, class Tree>
auto make_state(Tree&, io::properties ps) HM3_RETURNS(state<Tree, 1, idx>{ps});

template <class Tree, class State, class BCs> struct solver {
  using is_solver = std::true_type;
  using node_idx = node_idx_t<State>;
  using cell_idx = cell_idx_t<State>;
  using surface_idx = surface_idx_t<State>;
  using grid_idx = grid_idx_t<State>;

  static constexpr SInd nd() { return State::nd(); }
  static constexpr SInd nvars() { return State::nvars(); }
  static constexpr grid_idx idx() { return State::idx(); }

 private:
  Tree& t_;
  State& s_;
  BCs& b_;

 public:
  solver(Tree& t, State& s, BCs& bs) : t_(t), s_(s), b_(bs) {}

  auto tree() HM3_RETURNS(t_);
  auto tree() const HM3_RETURNS(t_);

  cell_idx create_cell() noexcept {
    const auto cIdx = cell_idx{s_.no_cells_};
    ++s_.no_cells_;
    return cIdx;
  }

  surface_idx create_surface() noexcept {
    auto sIdx = surface_idx{s_.no_surfaces_};
    ++s_.no_surfaces_;
    return sIdx;
  }

  auto cell_center(cell_idx i) HM3_RETURNS(s_.cell_center_.row(i));
  auto cell_center(cell_idx i) const HM3_RETURNS(s_.cell_center_.row(i));
};

template <class Tree, class State, class BCs>
auto make_solver(Tree& tree, State& state, BCs& bcs)
 HM3_RETURNS(solver<Tree, State, BCs>{tree, state, bcs});

template <class Geometry, class State> struct inflow : Geometry {
  State& s;
  inflow(Geometry& g, State& s_) : Geometry{g}, s{s_} {}
  Num operator()(cell_idx_t<State>) const { return 1.0; }
};

template <class Geometry, class State> struct outflow : Geometry {
  State& s;
  outflow(Geometry& g, State& s_) : Geometry{g}, s{s_} {}
  Num operator()(cell_idx_t<State>) const { return 1.0; }
};

template <class S>
auto create_cell(S& s, const node_idx_t<S> nIdx) -> cell_idx_t<S> {
  auto cIdx = s.create_cell();
  s.tree().cell(nIdx, grid_idx_t<S>{s.idx()}) = cIdx;
  s.cell_center(cIdx) = s.tree().coordinates(nIdx)();
  // cells().length(cIdx) = grid().cell_length(nIdx);
  // cells().bc_idx(cIdx) = invalid<SInd>();
  // const auto nghbrs
  //     = grid().all_samelvl_neighbors(node_idx(cIdx), solver_idx());
  // for (auto nghbrPos : grid().neighbor_positions()) {
  //   if (is_valid(nghbrs(nghbrPos))) {
  //     cells().neighbors(cIdx, nghbrPos) = nghbrs(nghbrPos);
  //   } else { cells().neighbors(cIdx, nghbrPos) = invalid<CellIdx>(); }
  // }
  // DBGV_ON((nIdx)(cIdx)(nghbrs));
  // add_cell_to_neighbors(cIdx);
  // for (auto nghbrPos : grid().neighbor_positions()) {
  //   const auto nghbrIdx = cells().neighbors(cIdx, nghbrPos);
  //   if (is_valid(nghbrIdx)) {
  //     cells().distances(cIdx, nghbrPos) = cell_dx(cIdx, nghbrIdx);
  //   }
  // }
  return cIdx;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
*/
