/*
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/fv/advection.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;
using namespace solver;

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_advection_1d", comm);
  io::session s(io::create, "fv_advection_1d", comm);

  // Grid parameters
  constexpr uint_t nd       = 1;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 1_u;
  auto max_grid_level       = min_grid_level + 0;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::box<nd>::unit();

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using physics_t = fv::advection<nd>;
  using solver_t  = fv::state<physics_t>;

  physics_t p{1.0};

  auto as0 = solver_t{g, 0_g, solver_block_capacity, p};

  /// Initial advection solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition
  for (auto&& b : as0.blocks()) {
    b.for_each_internal([&](auto c) {
      for (auto v : as0.physics.variables()) { b.variables(c.idx)(v) = 1.; }
    });
  }

  // for (auto timestep : view::iota(0, 10)) {
  //   // for (auto rkstep : fv::runge_kutta_steps()) {
  //   {  // can happen in parallel
  //     fv::clear_rhs(as0);
  //     fv::clear_halos(as0);
  //     fv::exchange_halos(as0);
  //   }
  //   // fv::apply_bounadry_conditions(as0, bcs);
  //   // auto dt = fv::compute_time_step(as0);
  //   // fv::set_time_step(as0, dt);
  //   fv::compute_gradients(as0);
  //   // fv::correct_boundary_gradients(as0, bcs);
  //   // fv::limit_gradients(as0, limiter_t);
  //   // {  // can happen in parallel
  //   //   {
  //   //     for (auto d : dimensions()) {
  //   //       fv::compute_fluxes(as0, d);
  //   //       fv::correct_boundary_fluxes(as0, bcs, d);
  //   //     }
  //   //   }
  //   //   fv::compute_source_terms(as0);
  //   // }
  //   // fv::update_rhs(as0);
  //   // }
  // }

  return test::result();
}
*/
int main() { return 0; }
