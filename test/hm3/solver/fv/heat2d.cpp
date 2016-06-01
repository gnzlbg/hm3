#include <hm3/geometry/sd/sphere.hpp>
#include <hm3/geometry/square.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/heat.hpp>
#include <hm3/solver/fv/models/heat/analytical_solution/slab.hpp>
#include <hm3/solver/fv/time_integration/euler_forward.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

template <typename T> struct dump;

using namespace hm3;
using namespace solver;
/*
struct CylBoundaryCondition {
  using sph_t = geometry::sd::fixed_sphere<2>;
  sph_t sph;

  CylBoundaryCondition() : sph(geometry::point<2>::constant(0.5), 0.25) {}

  template <typename Geom> bool is_cut(Geom&& g) const noexcept {
    return geometry::is_intersected(g, (*this));
  }

  template <typename Geom> bool is_inside(Geom&& g) const noexcept {
    return (*this)(centroid(g)) > 0. and !is_cut(g);
  }

  template <typename X> auto operator()(X&& x) const noexcept {
    return -sph(std::forward<X>(x));
  }

  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
      b.for_each([&](auto&& c) {
        auto x_c = b.center(c);
        auto g_c = b.geometry(c);
        if (!is_inside(g_c)) { to(b, c)(0) = 1.0; }
        if (!is_cut(g_c)) { return; }
        b.for_each_neighbor(c, [&](auto n) {
          auto x_n = b.center(n);
          if ((*this)(x_n) > 0.) { return; }
          geometry::point<2> x_bndry;
          x_bndry() = x_n() + (x_c() - x_n()) / 2.;
          to(b, n)(0) = ip::linear::interpolate(-1, 1.0, b.variables(c)(0));
        });
      });
    }
  }
};
*/
struct neumann_boundary_conditions {
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
      b.cells().for_each_halo([&](auto&& h_c) {
        auto i_c = b.closest_internal_cell(h_c);
        to(b, h_c) = b.variables(i_c);
      });
    }
  }
};

struct dirichlet_boundary_conditions {
  num_t val;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
      b.cells().for_each_halo([&](auto&& h_c) {
        auto x_hc = centroid(b.geometry()(h_c));
        if (x_hc(0) < 0. || x_hc(0) > 1.) {
          to(b, h_c)(0) = val;
        } else {
          auto i_c = b.cells().closest_internal_cell(h_c);
          to(b, h_c)(0) = b.variables(i_c)(0);
        }
      });
    }
  }
};

struct slab_boundary_conditions {
  num_t left;
  num_t right;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
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

struct dirichlet_boundary_conditionsQ {
  num_t val;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.tiles()) {
      b.cells().for_each_halo([&](auto&& h_c) {
        auto x_hc = centroid(b.geometry()(h_c));
        if (x_hc(0) < 0. || x_hc(0) > 1.) {
          to(b, h_c)(0) = val;
        } else {
          auto i_c = b.cells().closest_internal_cell(h_c);
          to(b, h_c)(0) = b.variables(i_c)(0);
        }
      });
      b.cells().for_each_internal([&](auto&& h_c) {
        auto x_hc                = centroid(b.geometry()(h_c));
        static constexpr auto nd = State::dimension();
        if (geometry::sd::sphere(x_hc, geometry::point<nd>::constant(0.5), 0.05)
            < 0.) {
          to(b, h_c)(0) = 2. * val;
        }
      });
    }
  }
};

struct laser_boundary_conditions {
  num_t val;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    int idx = 0;
    for (auto&& b : s.tiles()) {
      b.cells().for_each_halo([&](auto&& h_c) {
        auto x_hc = centroid(b.geometry()(h_c));
        HM3_ASSERT(x_hc == b.geometry().cell_centroid(h_c), "?");
        // std::cerr << "tidx: " << idx << " hc: " << h_c << " x_hc: " << x_hc
        //           << std::endl;
        // left, right, and bottom:
        if (x_hc(0) < 0. || x_hc(0) > 1. || x_hc(1) < 0.) {
          to(b, h_c)(0) = val;
          return;
        }
        if (x_hc(1) > 1.) {  // top:
          if (x_hc(0) < 0.35 || x_hc(0) > 0.65) {
            to(b, h_c)(0) = val;
          } else {
            to(b, h_c)(0) = 2. * val;
          }
        }
      });
      ++idx;
    }
  }
};

num_t error_slab_analytical(
 mpi::env& env, suint_t min_grid_level,
 solver::fv::heat::as::slab<2> const& analytic_solution) {
  auto comm        = env.world();
  std::string name = "fv_heat_2d_slab_analytical_error";

  // Initialize I/O session
  io::session::remove(name, comm);
  io::session s(io::create, name, comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;

  auto max_grid_level = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<nd>{});

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
  using method      = fv::default_method;

  using solver_t = fv::state<tile_layout, p, tint, method>;

  p physics;

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};

  /// Initial heat solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal(
     [&](auto&& c) { b.variables(c.idx())(0) = 20.0; });
  }

  /// Boundary conditions
  auto bcs = slab_boundary_conditions{0.0, 0.0};

  num_t cfl = 0.5;
  // solver::fv::vtk::serialize(as0, "result", 0, physics.cv());
  num_t time      = 0;
  num_t time_end  = 0.4;
  num_t time_step = 0;

  auto nf = fv::heat::flux::three_point;
  auto ts = fv::heat::time_step;
  auto li = fv::limiter::none;

  time_step
   = solver::fv::advance_until(as0, bcs, nf, ts, li, time, time_end, time_step,
                               cfl, [&](auto&& s, auto&& t, auto&& ts) {});
  // solver::fv::vtk::serialize(as0, "result", time_step, physics.cv());
  // solver::fv::vtk::serialize(as0, "block_result", time_step, physics.cv(),
  //                            4_gn);
  solver::fv::vtk::serialize(as0, "fv_heat_2d_slab_", min_grid_level,
                             physics.cv());

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
  solver::fv::vtk::serialize(as0, "fv_heat_2d_slab_analytic_", min_grid_level,
                             physics.cv());

  return error;
}

void convergence_slab_analytical(mpi::env& env) {
  /// Analytical solution
  solver::fv::heat::as::slab<2> analytic_solution([](num_t) {
    num_t initial_temperature = 20.;
    return initial_temperature;
  });

  std::vector<num_t> error;
  for (suint_t min_grid_level : {0, 1, 2, 3}) {
    error.push_back(
     error_slab_analytical(env, min_grid_level, analytic_solution));
  }

  std::vector<num_t> order(error.size());
  std::cerr << "e(0): " << error[0] << std::endl;
  for (std::size_t i = 1; i < error.size(); ++i) {
    order[i] = error[i - 1] / error[i];
    std::cerr << "e(" << i << "): " << error[i] << " o: " << order[i]
              << std::endl;
  }
}

void square(mpi::env& env) {
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_heat_2d", comm);
  io::session s(io::create, "fv_heat_2d", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 2_u;
  auto max_grid_level       = min_grid_level + 2;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::unit(geometry::square<nd>{});

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
  using method      = fv::default_method;

  // using tint     = fv::runge_kutta;
  using solver_t = fv::state<tile_layout, p, tint, method>;

  p physics;

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  for (auto n : {6, 8, 9, 11, 17, 26}) { g.refine(tree::node_idx{n}); }

  /// Initial heat solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal(
     [&](auto&& c) { b.variables(c.idx())(0) = 20.0; });
  }

  /// Boundary conditions
  // auto bcs = laser_boundary_conditions{20.0};
  auto bcs = dirichlet_boundary_conditionsQ{20.0};

  num_t cfl = 0.5;
  // bcs.apply(
  //  as0, [](auto&& b, auto&& c) -> decltype(auto) { return b.variables(c); });
  solver::fv::vtk::serialize(as0, "result", 0, physics.cv());
  num_t time      = 0;
  num_t time_end  = 0.4;
  num_t time_step = 0;

  // auto nf = fv::flux::local_lax_friedrichs;
  auto nf = fv::heat::flux::three_point;
  // auto nf = fv::heat::flux::rusanov;
  auto ts = fv::heat::time_step;
  auto li = fv::limiter::none;
  // auto li = fv::limiter::zero;
  // auto li = fv::limiter::none;

  // auto li = fv::limiter::minmod;
  // auto li = fv::limiter::zero;

  solver::fv::heat::as::slab<2> analytic([](num_t) { return 10.; }, 100);

  time_step = solver::fv::advance_until(
   as0, bcs, nf, ts, li, time, time_end, time_step, cfl,
   [&](auto&& s, auto&& t, auto&& ts) {
     // Initial condition by cell
     for (auto& b : as0.tiles()) {
       b.cells().for_each_internal([&](auto&& c) {
         b.variables(c.idx())(0) = analytic(b.geometry().cell_centroid(c), t);
       });
     }

     solver::fv::vtk::serialize(s, "result", ts, physics.cv());
   });
  solver::fv::vtk::serialize(as0, "result", time_step, physics.cv());
  solver::fv::vtk::serialize(as0, "block_result", time_step, physics.cv(),
                             4_gn);
}

/*
void sphere(mpi::env& env) {
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("fv_heat_2d", comm);
  io::session s(io::create, "fv_heat_2d", comm);

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

  using p    = fv::heat::physics<nd>;
  using tint = fv::euler_forward;
  // using tint     = fv::runge_kutta;
  using solver_t = fv::state<p, tint>;

  p physics;

  auto as0 = solver_t{g, 0_g, solver_block_capacity, physics};
  // g.refine(tree::node_idx{2});
  // g.refine(tree::node_idx{3});

  /// Initial heat solver grid:
  solver::initialize_grid(g, as0, [&](auto&& n) { return g.is_leaf(n); });

  // Initial condition by cell
  for (auto& b : as0.tiles()) {
    b.cells().for_each_internal([&](auto&& c) { b.variables(c.idx)(0) = 0.0; });
  }

  /// Boundary conditions
  auto bcs = CylBoundaryCondition();

  num_t cfl = 0.5;
  // bcs.apply(
  //  as0, [](auto&& b, auto&& c) -> decltype(auto) { return b.variables(c); });
  // solver::fv::vtk::serialize(as0, "result", 0, physics.cv());
  solver::fv::vtk::ls_serialize(as0, bcs, "ls_result", 0, physics.cv());
  num_t time      = 0;
  num_t time_end  = 0.2;
  num_t time_step = 0;

  // auto nf = fv::flux::local_lax_friedrichs;
  auto nf = fv::heat::flux::three_point;
  // auto nf = fv::heat::flux::rusanov;
  auto ts = fv::heat::time_step;
  auto li = fv::limiter::none;
  // auto li = fv::limiter::zero;
  // auto li = fv::limiter::none;

  // auto li = fv::limiter::minmod;
  // auto li = fv::limiter::zero;

  time_step
   = advance_until(as0, bcs, nf, ts, li, time, time_end, time_step, cfl);
  // solver::fv::vtk::serialize(as0, "result", time_step, physics.cv());
  // solver::fv::vtk::serialize(as0, "block_result", time_step, physics.cv(),
  //                            4_gn);
}
*/
int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);

  // square(env);
  convergence_slab_analytical(env);
  // sphere(env);

  return test::result();
}
