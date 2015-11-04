#include <hm3/geometry/sd.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/solver/fv/euler.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

template <typename T> struct dump;

using namespace hm3;
using namespace solver;

template <typename F> struct BoundaryConditions {
  F f;
  BoundaryConditions(F f_) : f(f_) {}
  template <typename Physics> constexpr void apply(fv::state<Physics>& s) {
    for (auto&& b : s.blocks()) {
      b.for_each_halo([&](auto&& c) { b.variables(c) = f(); });
    }
  }
};

template <typename F> auto make_bcs(F&& f) { return BoundaryConditions<F>(f); }

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_euler_2d", comm);
  io::session s(io::create, "fv_euler_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 0_u;
  auto max_grid_level       = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::square<nd>::unit();

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using p        = fv::euler<nd, fv::LxF>;
  using solver_t = fv::state<p>;

  p physics{1.4};

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  // g.refine(tree::node_idx{3});

  /// Initial advection solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  auto ic = [&](auto&& x, auto&& vars) {
    geometry::point<nd> xc = geometry::point<nd>::constant(0.5);
    auto r                 = 0.2;
    if (geometry::sd::sphere(x, xc, r) < 0) {
      // inside:
      vars(p::rho()) = 1.0;
      vars(p::p()) = 1.0;
    } else {
      // outside:
      vars(p::rho()) = 0.125;
      vars(p::p()) = 0.1;
    }
    p::vels(vars).fill(0.0);
    p::pv_to_cv(vars, physics.gamma_m1);
  };

  // Initial condition by cell
  for (auto& b : as0.blocks()) {
    b.for_each_internal([&](auto&& c) {
      ic(b.center(c), b.variables(c.idx));
    });
  }

  /// Boundary conditions
  auto bcs = make_bcs([&]() {
    num_a<p::no_variables()> vars;
    vars(p::rho()) = 0.125;
    vars(p::p()) = 0.1;
    p::vels(vars).fill(0.0);
    p::pv_to_cv(vars, physics.gamma_m1);
    return vars;
  });

  num_t cfl = 1.0;
  bcs.apply(as0);
  solver::fv::vtk::serialize(as0, "result_0", 0_gn);
  for (auto timestep : view::iota(1, 1000)) {
    //   // for (auto rkstep : fv::runge_kutta_steacdps()) {
    {  // can happen in parallel
      fv::clear_rhs(as0);
      fv::clear_halos(as0);  // in debug mode
    }
    {
      //  fv::cv_to_pv(as0);
      // fv::exchange_halos(as0);
    }
    bcs.apply(as0);
    num_t dt = fv::compute_time_step(as0, cfl);
    std::cout << "dt: " << dt << std::endl;
    // fv::compute_gradients(as0);
    //   // fv::correct_boundary_gradients(as0, bcs);
    //   // fv::limit_gradients(as0, limiter_t);
    fv::compute_internal_fluxes(as0, dt);
    // fv::compute_boundary_fluxes(as0, bcs);
    // fv::compute_source_terms(as0);
    // fv::pv_to_cv(as0);
    fv::update_rhs(as0);
    fv::advance_variables(as0, dt);
    // if (timestep % 10 == 0) {
    bcs.apply(as0);
    solver::fv::vtk::serialize(as0, "result_" + std::to_string(timestep), 0_gn);
    //}
  }

  return test::result();
}
