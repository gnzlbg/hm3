#ifdef FIXED
#include <hm3/geometry/intersect.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/solver/fv/fv.hpp>
#include <hm3/solver/fv/models/heat/numerical_flux.hpp>
#include <hm3/solver/fv/models/heat/physics.hpp>
#include <hm3/solver/fv/models/heat/time_step.hpp>
#include <hm3/solver/fv/time_integration.hpp>
#include <hm3/solver/fv/vtk.hpp>
#include <hm3/solver/utility.hpp>
#include <hm3/utility/test.hpp>

template <typename T> struct dump;

using namespace hm3;
using namespace solver;

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
    for (auto&& b : s.blocks()) {
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

/*
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

struct DirichletBoundaryConditions {
  num_t val;
  template <typename State, typename To>
  constexpr void apply(State& s, To&& to) {
    for (auto&& b : s.blocks()) {
      b.for_each_halo([&](auto&& h_c) {
        auto x_hc = b.center(h_c);
        if (x_hc(0) < 0. || x_hc(0) > 1.) {
          to(b, h_c)(0) = val;
        } else {
          auto i_c = b.closest_internal_cell(h_c);
          to(b, h_c) = b.variables(i_c);
        }
      });
    }
  }
};

void square(mpi::env& env) {
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
  auto bounding_box = geometry::square<nd>::unit();

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
  for (auto& b : as0.blocks()) {
    b.for_each_internal([&](auto&& c) { b.variables(c.idx)(0) = 0.0; });
  }

  /// Boundary conditions
  auto bcs = DirichletBoundaryConditions{1.0};

  num_t cfl = 0.5;
  // bcs.apply(
  //  as0, [](auto&& b, auto&& c) -> decltype(auto) { return b.variables(c); });
  solver::fv::vtk::serialize(as0, "result", 0, physics.cv());
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
  solver::fv::vtk::serialize(as0, "result", time_step, physics.cv());
  solver::fv::vtk::serialize(as0, "block_result", time_step, physics.cv(),
                             4_gn);
}
*/

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
  for (auto& b : as0.blocks()) {
    b.for_each_internal([&](auto&& c) { b.variables(c.idx)(0) = 0.0; });
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

int main(int argc, char* argv[]) {
  /// Initialize MPI
  mpi::env env(argc, argv);

  // square(env);
  sphere(env);

  return test::result();
}

#else
int main() { return 0; }
#endif
