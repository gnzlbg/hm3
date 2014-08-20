#pragma once
/// \file
///
/// Initializes a solver grid from tree with nodes that match a predicate

namespace hm3 {
namespace solver {

template <typename TreeGrid, typename Solver, typename Prediacte>
void initialize_grid(TreeGrid& g, Solver& s, Prediacte&& p) {
  for (auto&& n : g.nodes()) {
    if (p(n)) { s.push(n); }
  }
}

}  // namespace solver
}  // namespace hm3
