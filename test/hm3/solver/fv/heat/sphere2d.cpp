#include <hm3/geometry/box.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/heat.hpp>
#include <hm3/solver/fv/models/heat/analytical_solution/slab.hpp>
#include <hm3/solver/fv/time_integration/euler_forward.hpp>
#include <hm3/solver/fv/time_integration/runge_kutta.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

struct sphere_stl_bcs {
  triangulation_t boundary;
  num_t boundary_temperature = 20.;

  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    using std::experimental::visit;
    for (auto&& b : s.tiles()) {
      b.cells().for_each([&](auto&& c) {
        auto g_c = b.geometry()(c);

        auto result = intersect(g_c, boundary);

        switch (result.location) {
          case inside: {
            // mark as inside
            break;
          }
          case outside: {
            // mark as outside
            break;
          }
          case intersected: {
            // mark as intersected
            // reshape
            // mark inside neighbors as neighbors of intersected
            break;
          }
        }

        if (x_hc(0) < 0.) {
          to(b, h_c)(0) = left;
        } else if (x_hc(0) > 1.) {
          to(b, h_c)(0) = right;
        } else {  // neumann
          auto i_c = b.cells().closest_internal_cell(h_c);
          to(b, h_c)(0) = b.variables(i_c)(0);
        }
      });
    }
  }
};

int main(int argc, char* argv[]) {
  using namespace hm3;
  using namespace solver;

  /// Initialize MPI
  mpi::env env(argc, argv);

  auto comm                = env.world();
  string solver_name       = string("fv_heat_") + std::to_string(Nd) + "d";
  std::string session_name = solver_name + "_shpere_analytical_error";

  // Initialize I/O session
  io::session::remove(session_name, comm);
  io::session s(io::create, session_name, comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;

  auto min_grid_level = 3;
  auto max_grid_level = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::box<nd>{});

  using namespace grid::hierarchical;

  // Create the grid
  cm<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using tile_layout = fv::tile_layout<nd, 10, 2>;
  using p           = fv::heat::physics<nd>;
  using tint        = fv::euler_forward;
  using num_flux    = fv::heat::flux::three_point;
  using method      = fv::default_method;

  using solver_t = fv::state<tile_layout, p, tint, num_flux, method>;

  num_t thermal_diffusivity = 1.0;
  num_t initial_temperature;
  auto initial_condition = [=](num_t) { return initial_temperature; };
  p physics(thermal_diffusivity);

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};

  /// Initial heat solver grid:
  initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) {
      auto x                  = b.geometry().cell_centroid(c);
      b.variables(c.idx())(0) = initial_condition(x(0));
    });
  }

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

  return test::result();
}
