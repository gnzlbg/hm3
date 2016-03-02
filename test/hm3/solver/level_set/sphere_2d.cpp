/// \file
///
/// 2D level-set sphere test
#include <hm3/geometry/sd.hpp>
#include <hm3/grid/hierarchical/amr/criterion/level_at_distance.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/level_set/amr.hpp>
#include <hm3/solver/level_set/state.hpp>
#include <hm3/solver/utility.hpp>
#ifdef HM3_ENABLE_VTK
#include <hm3/grid/hierarchical/amr/serialization/vtk.hpp>
#include <hm3/grid/hierarchical/cartesian/serialization/multi_vtk.hpp>
#endif

using namespace hm3;
using grid::operator"" _g;

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("ls_sphere_2d", comm);
  io::session s(io::create, "ls_sphere_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 2;
  auto min_grid_level       = 4_u;
  auto max_grid_level       = min_grid_level + 3;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<2>{});

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  //.Make level set state for solvers 0 and 1:
  auto solver_node_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};
  auto ls0 = solver::level_set::state<nd>{g, 0_g, solver_node_capacity, s};
  auto ls1 = solver::level_set::state<nd>{g, 1_g, solver_node_capacity, s};

  /// Initial solver 0 grid:
  solver::initialize_grid(g, ls0, [&](auto&& n) {
    return g.is_leaf(n) and ls0.g.tree().coordinates(n)(0) < 0.5;
  });
  /// Initial solver 1 grid:
  solver::initialize_grid(g, ls1, [&](auto&& n) {
    return g.is_leaf(n) and ls1.g.tree().coordinates(n)(0) > 0.5;
  });

  /// Generate moving spheres
  const num_t radius = 0.1;
  auto sphere0       = geometry::sd::moving_sphere<nd>{radius};
  auto sphere1       = geometry::sd::moving_sphere<nd>{radius};

  using geometry::point;
  auto sphere_center0 = [](auto&& step) {
    auto x = point<nd>{point<nd>::c(0.5)};
    x(1) += 0.25 * std::sin(step);
    return x;
  };
  auto sphere_center1 = [](auto&& step) {
    auto x = point<nd>{point<nd>::c(0.5)};
    x(1) += 0.25 * std::cos(step);
    return x;
  };

  /// Adaptive mesh refinement around the spheres
  // Create an amr action to adapt the grid around the moving sphere ensuring a
  // minimum grid level up to a certain distance:
  auto amr_criterion
   = amr::criterion::level_till_cell_distances{g,
                                               {{10000, min_grid_level},
                                                {5, min_grid_level + 1},
                                                {5, min_grid_level + 2},
                                                {5, min_grid_level + 3}}};

  /// Create the AMR actions to perform
  auto amr_action0
   = [&](auto&& n) { return amr_criterion(g, sphere0, ls0.g.tree_node(n)); };
  auto amr_action1
   = [&](auto&& n) { return amr_criterion(g, sphere1, ls1.g.tree_node(n)); };

  // Create an adaptive mesh refinement handler for adapting the grid
  auto amr_handler0 = solver::level_set::make_amr(ls0);
  auto amr_handler1 = solver::level_set::make_amr(ls1);

  /// Generate some time steps
  auto no_time_steps = 4;
  auto time_steps    = view::generate([]() mutable {
                      static num_t next = 0.;
                      auto r            = next;
                      next += 1. / 4. * math::pi<num_t>;
                      return r;
                    })
                    | view::take(no_time_steps);

  solver::sort(g, ls0, ls1);
  ls0.set_node_values(sphere0);
  ls1.set_node_values(sphere1);
  solver::write(g, ls0, ls1);

  /// Main loop:
  uint_t count_ = 0;
  RANGES_FOR (auto&& t, time_steps) {
    string root_name0 = "amr_state_ls0_" + std::to_string(count_);
    // Update sphere centers
    sphere0.centroid = sphere_center0(t);
    sphere1.centroid = sphere_center1(t);
    ls0.set_node_values(sphere0);
    ls1.set_node_values(sphere1);

    int iter_counter = 0;
    while (amr_handler0.adapt(amr_action0, [&]() {
#ifdef HM3_ENABLE_VTK
      string fname = root_name0 + "_it_" + std::to_string(iter_counter);
      amr::vtk::serialize(amr_handler0, fname);

      grid::hc::vtk::serialize(g, "ls_grid_" + std::to_string(count_) + "_"
                                   + std::to_string(iter_counter));
#endif
      iter_counter++;
    })) {
      ls0.set_node_values(sphere0);
    }

    while (amr_handler1.adapt(amr_action1)) { ls1.set_node_values(sphere1); }

    solver::sort(g, ls0, ls1);
    solver::write(g, ls0, ls1);
    count_++;
  }

  return 0;
}
