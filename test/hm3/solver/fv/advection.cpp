/// \file advection.cpp
#ifdef HM3_OLD_HT
#include <tuple>
#include <hm3/solver/fv.hpp>

#include <hm3/core/mpi.hpp>
#include <hm3/core/test.hpp>
#include <hm3/grid/hierarchical_cartesian.hpp>
#include <hm3/grid/generator.hpp>
#include <hm3/geometry/implicit.hpp>

using namespace hm3;

int main(int argc, char* argv[]) {
  mpi::env env(argc, argv);

  static constexpr SInd nd = 1;

  /// Create the tree:
  io::properties tree_ps;
  io::insert<Ind>(tree_ps, "maxNoNodes", 20);

  /// Bounding box:
  auto pL           = geometry::point<nd>{0.};
  auto pR           = geometry::point<nd>{1.};
  auto bounding_box = geometry::square<nd>{pL, pR};
  io::insert<geometry::square<nd>>(tree_ps, "bounding_box", bounding_box);

  grid::hierarchical_cartesian<nd, 1> tree(tree_ps);

  /// Boundaries:
  auto eL = geometry::implicit::edge<nd>{pL, geometry::vector<nd>{1.}};
  auto eR = geometry::implicit::edge<nd>{pR, geometry::vector<nd>{-1.}};

  /// Initial domain:
  auto initial_domain = geometry::implicit::take_intersection(eL, eR);

  /// Create the solver:
  static constexpr SInd solver_idx = 0;
  io::properties solver_ps;
  io::insert<Ind>(solver_ps, "max_no_cells", 20);

  /// Create the solver state:
  auto fv_state = solver::fv::make_state<solver_idx>(tree, solver_ps);

  /// Create the boundary conditions:
  auto iBC = solver::fv::inflow<decltype(eL), decltype(fv_state)>{eL, fv_state};
  auto oBC
   = solver::fv::outflow<decltype(eR), decltype(fv_state)>{eR, fv_state};

  // auto bcs = std::make_tuple(iBC, oBC);
  // /// Create the solver:
  // auto fv_solver = solver::fv::make_solver(tree, fv_state, bcs);

  /// Generate the tree:
  auto g = grid::generator<nd>(4);
  g.generate(tree, [&](auto nIdx) {
    return tree.level(nIdx) == 3 || grid::outside(tree, nIdx, initial_domain);
  });

  // /// Generate the solver grid:
  // g.generate(fv_solver, [&](auto nIdx) { return tree.level(nIdx) == 3; });

  return test::result();
}
#else
int main() { return 0; }
#endif
