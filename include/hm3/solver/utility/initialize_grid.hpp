#pragma once
/// \file initialize_grid.hpp

namespace hm3 {
namespace solver {

template <typename TreeGrid, typename Solver, typename Prediacte>
void initialize_grid(TreeGrid& g, Solver& s, Prediacte&& p) {
  for (auto&& n : g.nodes() | g.leaf()) {
    if (p(n)) { s.push(n); }
  }
}

}  // namespace solver
}  // namespace hm3
