#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/heat.hpp>
#include <hm3/solver/fv/models/heat/analytical_solution/slab.hpp>
#include <hm3/solver/fv/time_integration/euler_forward.hpp>
#include <hm3/solver/fv/time_integration/runge_kutta.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/solver/utility/aligned_boundary.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

constexpr dim_t nd = 2;

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);

  auto comm                = env.world();
  string solver_name       = string("fv_heat_") + std::to_string(nd) + "d";
  std::string session_name = solver_name + "_shpere_analytical_error";

  // Initialize I/O session
  io::session::remove(session_name, comm);
  io::session s(io::create, session_name, comm);

  // Grid parameters
  constexpr uint_t no_grids = 1;

  auto min_grid_level = 3;
  auto max_grid_level = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};

  // Geometry
  using p_t = geometry::point<nd>;
  using v_t = geometry::vec<nd>;

  // Create aligned boundary lines
  using geometry::line;
  line<nd> left_boundary_line(p_t{0., 0.5}, v_t{0.0, 1.0});
  line<nd> right_boundary_line(p_t{1., 0.5}, v_t{0.0, -1.0});
  line<nd> top_boundary_line(p_t{0., 0.75}, v_t{1.0, 0.0});
  line<nd> bottom_boundary_line(p_t{0., 0.25}, v_t{-1.0, 0.0});

  // Construct the aligned boundaries
  using solver::aligned_boundary;
  using solver::boundary_idx;
  aligned_boundary<nd> left_boundary(left_boundary_line, boundary_idx{0});
  aligned_boundary<nd> right_boundary(right_boundary_line, boundary_idx{1});
  aligned_boundary<nd> top_boundary(top_boundary_line, boundary_idx{2});
  aligned_boundary<nd> bottom_boundary(bottom_boundary_line, boundary_idx{3});

  using solver::domain;
  domain<nd> domain0;
  domain0.aligned_boundaries.push_back(left_boundary);
  domain0.aligned_boundaries.push_back(right_boundary);
  domain0.aligned_boundaries.push_back(top_boundary);
  domain0.aligned_boundaries.push_back(bottom_boundary);

  auto bounding_box = geometry::box<nd>::unit();

  // Create the grid
  using namespace grid::hierarchical;
  cm<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  namespace fv = solver::fv;

  using tile_layout = fv::tile_layout<nd, 10, 2>;
  using p           = fv::heat::physics<nd>;
  using tint        = fv::euler_forward;
  using num_flux    = fv::heat::flux::three_point_fn;
  using method      = fv::default_method;

  using solver_t = fv::state<tile_layout, p, tint, num_flux, method>;

  num_t thermal_diffusivity = 1.0;
  num_t initial_temperature = 0.0;
  auto initial_condition    = [&](num_t) { return initial_temperature; };
  p physics(thermal_diffusivity);

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};

  /// Initial heat solver grid:
  solver::initialize_leaf_grid_domain(g, as0, domain0);

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) {
      auto x                  = b.geometry().cell_centroid(c);
      b.variables(c.idx())(0) = initial_condition(x(0));
    });
  }

  /*
  num_t cfl       = 0.1;
  num_t time      = 0;
  num_t time_end  = 0.4;
  num_t time_step = 0;

  auto ts = fv::heat::time_step;
  auto li = fv::limiter::none;

  time_step = fv::advance_until(as0, bcs, ts, li, time, time_end, time_step,
                                cfl, [&](auto&& s, auto&& t, auto&& ts) {});
  fv::vtk::serialize(as0, solver_name + "_", min_grid_level, physics.cv());

  /// Compute Error
  num_t error = 0.;
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) {
      auto x           = b.geometry().cell_centroid(c);
      auto volume      = b.geometry().cell_volume();
      auto t_analytic  = analytic_solution(x, time_end);
      auto t_numerical = b.variables(c.idx())(0);
      error += volume * std::abs(t_analytic - t_numerical);
    });
  }

  /// Write analytical solution (overwritte cell variables)
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) {
      auto x                  = b.geometry().cell_centroid(c);
      auto volume             = b.geometry().cell_volume();
      auto t_analytic         = analytic_solution(x, time_end);
      b.variables(c.idx())(0) = t_analytic;
    });
  }
  fv::vtk::serialize(as0, "fv_heat_2d_slab_analytic_", min_grid_level,
                     physics.cv());
  */
  return test::result();
}
