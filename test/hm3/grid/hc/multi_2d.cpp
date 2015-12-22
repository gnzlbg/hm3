/// \file
///
/// Test multi hierarchical Cartesian grid 2D
#include <hm3/amr/amr.hpp>
#include <hm3/amr/criterion/level_at_distance.hpp>
#include <hm3/grid/hc/amr/multi.hpp>
#include <hm3/grid/hc/multi.hpp>
#ifdef HM3_ENABLE_VTK
#include <hm3/amr/serialization/vtk.hpp>
#endif
#include <hm3/geometry/sd.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/utility/test.hpp>
#include "grid.hpp"

using namespace hm3;
using tree::node_idx;

int main(int argc, char* argv[]) {
  // Initialize MPI
  mpi::env env(argc, argv);
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("hc_multi_2d_amr", comm);
  io::session s(io::create, "hc_multi_2d_amr", comm);

  // Grid parameters
  constexpr uint_t nd = 2;
  auto min_grid_level = 4_u;
  auto max_grid_level = min_grid_level + 3;
  auto node_capacity
   = node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<2>{});
  auto no_grids     = 2;

  // Create the grid
  grid::hc::multi<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  // Resort and write the initial grid (file idx: 0)
  g.sort();
  g.write();

  // Generate moving sphere
  using hm3::math::pi;
  using geometry::point;
  auto time_steps = view::generate([]() mutable {
                      static num_t next = 0.;
                      auto r            = next;
                      next += 1. / 2. * pi<num_t>;
                      return r;
                    })
                    | view::take(2);
  const num_t radius = 0.15;
  auto moving_sphere = geometry::sd::moving_sphere<nd>{radius};
  auto sphere_center = [&](num_t step) {
    return point<nd>{point<nd>::c(0.5) + point<nd>::c(0.25 * std::sin(step))};
  };

  // Create an adaptive mesh refinement handler for adapting the grid
  auto amr_handler = amr::make(g);

  // Create an amr action to adapt the grid around the moving sphere ensuring a
  // minimum grid level up to a certain distance:
  auto amr_criterion
   = amr::criterion::level_till_cell_distances{g,
                                               {{10000, min_grid_level},
                                                {5, min_grid_level + 1},
                                                {5, min_grid_level + 2},
                                                {5, min_grid_level + 3}}};
  auto amr_action
   = [&](auto&& n) { return amr_criterion(g, moving_sphere, n); };

  // Loop over all time steps, moving the sphere, and adapting the grid
  int count_ = 0;
  RANGES_FOR (auto&& t, time_steps) {
    string root_name       = "amr_state_ts_" + std::to_string(count_);
    moving_sphere.centroid = sphere_center(t);
    int iter_counter       = 0;
    while (amr_handler.adapt(amr_action, [&]() {
      // If VTK is enabled write the state of the AMR handler at each iteration
      // to
      // vtu format (useful for debugging):
      // #ifdef HM3_ENABLE_VTK
      //       string fname = root_name + "_it_" + std::to_string(iter_counter);
      //       amr::vtk::serialize(amr_handler, fname);
      // #endif
      iter_counter++;
    })) {
      g.sort();
      test::consistency_checks(g);
    }
    g.write();
    count_++;
  }

  return test::result();
}
