#pragma once
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

namespace hm3 {

struct slab_boundary_conditions {
  num_t left;
  num_t right;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
      // TODO: should be for_each_boundary_cell...
      b.cells().for_each_halo([&](auto&& h_c) {
        auto x_hc = centroid(b.geometry()(h_c));
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

template <dim_t Nd, typename IC, typename TimeInt, typename NumFlux>
num_t error_slab_analytical(
 mpi::env& env, suint_t min_grid_level,
 solver::fv::heat::as::slab<2> const& analytic_solution, IC initial_condition,
 num_t thermal_diffusivity, slab_boundary_conditions bcs, TimeInt, NumFlux,
 bool unstructured) {
  using namespace solver;
  auto comm                = env.world();
  string solver_name       = string("fv_heat_") + std::to_string(Nd) + "d";
  std::string session_name = solver_name + "_slab_analytical_error";

  // Initialize I/O session
  io::session::remove(session_name, comm);
  io::session s(io::create, session_name, comm);

  // Grid parameters
  constexpr uint_t nd       = Nd;
  constexpr uint_t no_grids = 1;

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
  using tint        = TimeInt;
  using num_flux    = NumFlux;
  using method      = fv::default_method;

  using solver_t = fv::state<tile_layout, p, tint, num_flux, method>;

  p physics(thermal_diffusivity);

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};

  if (unstructured) {
    switch (min_grid_level) {
      case 0: {
        break;
      }
      case 1: {
        g.refine(tree::node_idx{1});
        break;
      }
      case 2: {
        g.refine(tree::node_idx{8});
        break;
      }
      case 3: {
        g.refine(tree::node_idx{36});
        break;
      }
      default: { HM3_FATAL_ERROR("unstructured refinement unimplemented"); }
    }
  }

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

  return error;
}

template <dim_t Nd, typename TimeInt, typename NumFlux>  //
void convergence_slab_analytical(mpi::env& env, TimeInt time_int,
                                 NumFlux num_flux, num_t order_should,
                                 bool unstructured = false) {
  using namespace solver;
  num_t thermal_diffusivity = 1.5;
  num_t initial_temperature = 20.;
  auto initial_condition = [=](num_t) { return initial_temperature; };

  /// Boundary conditions
  num_t temperature_left  = 0.0;
  num_t temperature_right = 0.0;
  auto bcs = slab_boundary_conditions{temperature_left, temperature_right};

  /// Analytical solution
  fv::heat::as::slab<Nd> analytic_solution(initial_condition);
  analytic_solution.temperature_left    = temperature_left;
  analytic_solution.temperature_right   = temperature_right;
  analytic_solution.thermal_diffusivity = thermal_diffusivity;

  std::vector<num_t> error;
  for (suint_t min_grid_level : {0, 1, 2}) {
    error.push_back(error_slab_analytical<Nd>(
     env, min_grid_level, analytic_solution, initial_condition,
     thermal_diffusivity, bcs, time_int, num_flux, unstructured));
  }

  std::vector<num_t> order(error.size());
  std::cerr << "e(0): " << error[0] << std::endl;
  for (std::size_t i = 1; i < error.size(); ++i) {
    order[i] = error[i - 1] / error[i];
    std::cerr << "e(" << i << "): " << error[i] << " o: " << order[i]
              << std::endl;
    CHECK(order[i] > order_should);
  }
}

}  // namespace hm3
