#include <hm3/geometry/square.hpp>
#include <hm3/grid/hierarchical/amr/amr.hpp>
#include <hm3/grid/hierarchical/amr/criterion/level_at_distance.hpp>
#include <hm3/grid/hierarchical/cartesian/amr/multi.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/grid/hierarchical/grid.hpp>
#include <hm3/solver/fv/euler.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/euler/initial_condition/shock_tube.hpp>
#include <hm3/solver/fv/numerical_flux/rusanov.hpp>
#include <hm3/solver/fv/time_integration.hpp>
#include <hm3/solver/fv/time_integration.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/mpi.hpp>

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

template <typename Parameters>
auto unit_square_neumann(hm3::mpi::env& env, Parameters p) {
  using namespace hm3;
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_euler_sod_2d", comm);
  io::session s(io::create, "fv_euler_sod_2d", comm);

  constexpr suint_t nd       = Parameters::nd;
  constexpr suint_t no_grids = 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, p.level)};
  auto bounding_box = geometry::unit(geometry::square<nd>{});

  using namespace grid;
  using namespace grid::hierarchical;

  // Create the grid
  cm<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  generation::uniform(g, p.level);

  auto tile_capacity = grid_node_idx{*node_capacity};

  using physics_t  = typename Parameters::physics;
  using time_int_t = typename Parameters::time_integration;
  using solver_t   = solver::fv::state<physics_t, time_int_t>;

  physics_t physics{p.gamma};

  solver_t fv_solver{g, 0_g, tile_capacity, physics};

  // Initialize solver grid with the leaf nodes of the tree:
  solver::initialize_grid(g, fv_solver, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition by cell
  for (auto& b : fv_solver.blocks()) {
    b.for_each_internal([&](auto&& c) {
      auto pvs           = p.initial_condition(b.centroid(c));
      b.variables(c.idx) = physics.pv().to_cv(pvs);
    });
  }

  // Boundary conditions
  auto bcs = NeumannBoundaryConditions{};
  bcs.apply(fv_solver, [](auto&& b, auto&& c) -> decltype(auto) {
    return b.variables(c);
  });

  p.time = 0.;
  p.dt   = 0;

  solver::fv::vtk::serialize(fv_solver, p.name, p.dt, physics.cv());

  p.dt = advance_until(fv_solver, bcs, p.numerical_flux, p.time_step, p.limiter,
                       p.time, p.time_end, p.dt, p.cfl);
  solver::fv::vtk::serialize(fv_solver, p.name, p.dt, physics.cv());

  return std::make_tuple(p, std::move(fv_solver));
}
