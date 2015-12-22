/*#include <hm3/geometry/sd.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/advection/initial_condition/square.hpp>
#include <hm3/solver/fv/models/advection/numerical_flux/upwind.hpp>
#include <hm3/solver/fv/models/advection/physics.hpp>
#include <hm3/solver/fv/numerical_flux/rusanov.hpp>
#include <hm3/solver/fv/time_integration.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

template <typename T> struct dump;

using namespace hm3;
using namespace solver;

struct NeumannBoundaryConditions {
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.blocks()) {
      b.for_each_halo([&](auto&& h_c) {
        auto i_c = b.closest_internal_cell(h_c);
        to(b, h_c) = b.variables(i_c);
      });
    }
  }
};

void square(mpi::env& env) {
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_euler_sod_2d", comm);
  io::session s(io::create, "fv_euler_sod_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 0_u;
  auto max_grid_level       = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<nd>{});

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using p    = fv::advection::physics<nd>;
  using tint = fv::euler_forward;
  // using tint     = fv::runge_kutta;
  using solver_t = fv::state<p, tint>;

  num_a<nd> vel = num_a<nd>::Zero();
  vel(0)        = 1.0;
  p physics(vel);

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  // g.refine(tree::node_idx{2});
  // g.refine(tree::node_idx{3});

  /// Initial advection solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  auto ic = fv::advection::ic::square<nd>(
   geometry::square<nd>(geometry::point<nd>::constant(0.25), 0.1), 1.0, 0.1);

  // Initial condition by cell
  for (auto& b : as0.blocks()) {
    b.for_each_internal(
     [&](auto&& c) { b.variables(c.idx) = ic(b.centroid(c)); });
  }

  /// Boundary conditions

  auto box = geometry::sd::box<nd>(geometry::point<nd>::constant(0.5), 1.0);

  auto bcs = NeumannBoundaryConditions{};

  num_t cfl = 1.0;
  // bcs.apply(
  //  as0, [](auto&& b, auto&& c) -> decltype(auto) { return b.variables(c); });
  solver::fv::vtk::serialize(as0, "result", 0, physics.cv());
  num_t time      = 0;
  num_t time_end  = 0.2;
  num_t time_step = 0;

  // auto nf = fv::flux::local_lax_friedrichs;
  auto nf = fv::advection::flux::upwind;
  // auto nf = fv::advection::flux::rusanov;
  auto ts = fv::advection::time_step;
  auto li = fv::limiter::van_albada;
  // auto li = fv::limiter::zero;
  // auto li = fv::limiter::none;

  // auto li = fv::limiter::minmod;
  // auto li = fv::limiter::zero;

  time_step
   = advance_until(as0, bcs, nf, ts, li, time, time_end, time_step, cfl);
  solver::fv::vtk::serialize(as0, "result", time_step, physics.cv());
  solver::fv::vtk::serialize(as0, "block_result", time_step, physics.cv(),
                             4_gn);
}

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);

  square(env);

  // explosion_test(env);

  return test::result();
}
*/

int main() { return 0; }
