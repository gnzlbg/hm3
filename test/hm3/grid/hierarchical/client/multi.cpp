/// \file
///
/// Hierarchical multi-tree client tests
#include <hm3/grid/hierarchical/client/multi.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

using namespace grid;
using namespace hierarchical;

int main(int argc, char* argv[]) {
  /// \name Setup
  ///@{
  /// Initialize MPI
  mpi::env env(argc, argv);
  auto comm = env.world();

  /// Initialize I/O session
  io::session::remove("state_grid", comm);
  io::session s(io::create, "state_grid", comm);

  /// Grid parameters
  constexpr uint_t nd = 2;
  auto max_grid_level = 2;
  auto node_capacity
   = tree_node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<2>{});
  auto no_grids     = 2;

  /// Create the grid
  cartesian::multi<nd> g(s, node_capacity, no_grids, bounding_box);

  /// Refine the grid up to the maximum leaf node level
  generation::uniform(g, max_grid_level);
  ///@}  // Setup

  client::multi<nd> grid_state0(g, 0_g, *node_capacity);
  client::multi<nd> grid_state1(g, 1_g, *node_capacity);
  CHECK(grid_state0.idx() == 0_g);
  CHECK(grid_state1.idx() == 1_g);

  auto no_leaf_nodes = distance(g.nodes() | g.leaf());
  auto append_nodes  = [&](auto&& state, uint_t from, uint_t to) {
    uint_t counter = 0;
    for (auto&& n : g.nodes()) {
      if (counter >= from and counter < to) { state.push(n); }
      ++counter;
    }
  };

  // Make grid nodes [0, no_leafs/2) part of state0 and from [no_leafs/2,
  // no_leafs) part of state1:
  append_nodes(grid_state0, 0, no_leaf_nodes / 2);
  append_nodes(grid_state1, no_leaf_nodes / 2, no_leaf_nodes);

  CHECK(grid_state0 != grid_state1);
  CHECK(!(grid_state0 == grid_state1));
  CHECK(grid_state0 == grid_state0);

  test::check_equal(grid_state0.tree_nodes(),
                    boxed_ints<tree_node_idx>(0, no_leaf_nodes / 2));
  // test::check_equal(
  //  grid_state1.nodes(),
  //  boxed_ints<tree_node_idx>(no_leaf_nodes / 2, no_leaf_nodes));

  // test neighbors
  // test children
  // test refine / coarsen
  // test swap
  // test i/o

  return test::result();
}
