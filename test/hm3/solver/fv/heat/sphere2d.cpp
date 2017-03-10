#ifdef FIXED
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/math/integral/gauss_legendre.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/heat.hpp>
#include <hm3/solver/fv/models/heat/analytical_solution/slab.hpp>
#include <hm3/solver/fv/time_integration/euler_forward.hpp>
#include <hm3/solver/fv/time_integration/runge_kutta.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/solver/utility/domain.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

template <typename... Args>
struct boundary_conditions {
  std::tuple<Args...> bcs;
};

template <typename... Args>
boundary_conditions<Args...> make_bcs(Args&&... args) {
  return std::make_tuple(args...);
}

template <typename Domain, typename BCs>
struct boundary_condition_handler {
  Domain domain;
  BCs bcs;

  template <typename State>
  void reinitialize_cut_cells(State& s) const {
    for (auto&& t : s.tiles()) {
      // If the tile doesn't intersect the bounding box of any of the boundaries
      // => skip it
      if (not domain.intersects_boundary(
           t.geometry().tile_external_bounding_box())) {
        continue;
      }

      int tile_boundary_cell_count  = 0;
      int total_boundary_cell_count = 0;
      for (auto cidx : t.cells().all()) {
        auto cg = t.geometry()(cidx);
        if (not domain.intersects_boundary(cg)) { continue; }

        // domain intersects cell:
        ++tile_boundary_cell_count;

        // split the cell intersected with the domain
        // auto bcells = t.create_boundary_cells(cidx, cg);
        // HM3_ASSERT(bcells.size() > 0, "");
        // domain.reshape_cells(bcells);
        // HM3_ASSERT(bcells.size() > 0, "");

        /*
        // add the split cells inside the domain as boundary cells
        for (auto&& bcell_g : split_cells) {
          if (geometry::is_outside(domain.relative_position(bcell_g))) {
            continue;
          }
          // the split cell is inside the domain:
          ++total_boundary_cell_count;

          // the tile contains a boundary cell
          t.cell_type(cidx) = fv_cell_t::boundary_cell;

          // create a new boundary cell:
          auto& bc = t.create_new_bc(cidx);

          // add boundary cell faces
          for (auto&& f : faces(bcell_g)) {
            // if intersects any of the original cell faces over a "surface":
            //  - find which face
            //  - find the neighbor of cidx in that direction
            //  - create a boundary surface:
            //    - with internal neighbor
            //    - add surface geometry
            //    - compute centroid, normal, area
          }
        }
        */
      }
    }
  }
};

template <typename Domain, typename BCs>
boundary_condition_handler<Domain, BCs> make_bch(Domain&& d, BCs&& bcs) {
  return {d, bcs};
}

template <typename F>
struct boundary_condition {
  solver::boundary_idx idx;
  F&& apply;
};

template <typename F>
boundary_condition<F> make_bc(solver::boundary_idx idx, F&& f) {
  return {idx, std::forward<F>(f)};
}

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
  using geometry::plane;
  plane<nd> left_boundary_plane(p_t{0., 0.5}, v_t{0.0, 1.0});
  plane<nd> right_boundary_plane(p_t{1., 0.5}, v_t{0.0, -1.0});
  plane<nd> top_boundary_plane(p_t{0., 0.75}, v_t{1.0, 0.0});
  plane<nd> bottom_boundary_plane(p_t{0., 0.25}, v_t{-1.0, 0.0});

  // Construct the aligned boundaries
  using solver::aligned_boundary;
  using solver::boundary_idx;
  aligned_boundary<nd> left_boundary(left_boundary_plane, boundary_idx{0});
  aligned_boundary<nd> right_boundary(right_boundary_plane, boundary_idx{1});
  aligned_boundary<nd> top_boundary(top_boundary_plane, boundary_idx{2});
  aligned_boundary<nd> bottom_boundary(bottom_boundary_plane, boundary_idx{3});

  // Construct the computational domain
  using solver::domain;
  domain<nd> domain0;
  domain0.aligned_boundaries.push_back(left_boundary);
  domain0.aligned_boundaries.push_back(right_boundary);
  domain0.aligned_boundaries.push_back(top_boundary);
  domain0.aligned_boundaries.push_back(bottom_boundary);

  auto isothermal_boundary_fn = [](auto&& State, auto&& Tile, auto&& Surface) {

  };

  auto bc0 = make_bc(boundary_idx{0}, isothermal_boundary_fn);
  auto bc1 = make_bc(boundary_idx{1}, isothermal_boundary_fn);
  auto bc2 = make_bc(boundary_idx{2}, isothermal_boundary_fn);
  auto bc3 = make_bc(boundary_idx{3}, isothermal_boundary_fn);

  auto bcs0 = std::make_tuple(bc0, bc1, bc2, bc3);  // TODO: make bc group?

  auto bcs = make_bch(domain0, bcs0);

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
  using method      = fv::cut_cell_grad_method;

  using solver_t = fv::state<tile_layout, p, tint, num_flux, method>;

  num_t thermal_diffusivity = 1.0;
  num_t initial_temperature = 0.0;
  auto initial_condition
   = [&](geometry::point<nd>) { return initial_temperature; };
  p physics(thermal_diffusivity);

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};

  /// Initial heat solver grid:
  solver::initialize_leaf_grid_domain(g, as0, domain0);

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) {
      auto cell_geometry = b.geometry()(c);
      b.variables(c.idx())(0)
       = math::integral_gauss_legendre(initial_condition, cell_geometry);
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

  return test::result();
}
#endif
int main() { return 0; }
