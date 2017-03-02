#pragma once
/// \file
///
/// Initializes a solver grid from tree with nodes that match a predicate
#include <hm3/geometry/algorithm/relative_position.hpp>

namespace hm3 {
namespace solver {

template <typename TreeGrid, typename Solver, typename Prediacte>
void initialize_grid(TreeGrid& g, Solver& s, Prediacte&& p) {
  for (auto&& n : g.nodes()) {
    if (p(n)) { s.push(n); }
  }
}

template <typename TreeGrid, typename Solver, typename Predicate>
void initialize_leaf_grid(TreeGrid& g, Solver& s, Predicate&& p) {
  initialize_grid(g, s, [&](auto&& n) { return g.is_leaf(n) and p(n); });
}

template <typename TreeGrid, typename Solver, typename Domain>
void initialize_leaf_grid_domain(TreeGrid& g, Solver& s, Domain&& d) {
  initialize_leaf_grid(g, s, [&](auto&& n) {
    return not geometry::is_outside(d.relative_position(g.geometry(n)));
  });
}

}  // namespace solver
}  // namespace hm3
