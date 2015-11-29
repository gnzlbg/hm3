#include <hm3/io/session.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/geometry/intersect.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/solver/lbm/state.hpp>
#include <hm3/solver/lbm/lattice/d2q9_2.hpp>
#include <hm3/solver/lbm/vtk.hpp>
#include <hm3/solver/lbm/lbm.hpp>
#include <hm3/solver/utility.hpp>

using namespace hm3;
using namespace solver;

struct BoundaryConditions {
  using sph_t = geometry::sd::fixed_sphere<2>;
  sph_t sph;

  num_t density;
  num_t accel;
  num_t t_1;
  num_t t_2;

  BoundaryConditions(num_t rho, num_t a)
   : sph(geometry::point<2>::constant(0.5), 0.05), density(rho), accel(a) {
    t_1 = density * accel / 9.;
    t_2 = density * accel / 36.;
  }

  template <typename Geom> bool is_cut(Geom&& g) const noexcept {
    return geometry::is_intersected(g, (*this));
  }

  template <typename Geom> bool is_inside(Geom&& g) const noexcept {
    return (*this)(centroid(g)) > 0. and !is_cut(g);
  }

  template <typename X> auto operator()(X&& x) const noexcept {
    return sph(std::forward<X>(x));
  }

  // inflow
  template <typename State> void redistribute(State&& s) {
    for (auto&& b : s.blocks()) {
      b.for_each_internal([&](auto c) {
        if (c.x[0] == 2 and c.x[1] != 2 and c.x[1] != 101
            and (b.nodes0(c, 3) - t_1 > 0. || b.nodes0(c, 6) - t_2 > 0.
                 || b.nodes0(c, 7) - t_2 > 0.)) {
          b.nodes0(c, 1) += t_1;
          b.nodes0(c, 5) += t_2;
          b.nodes0(c, 8) += t_2;

          b.nodes0(c, 3) -= t_1;
          b.nodes0(c, 6) -= t_2;
          b.nodes0(c, 7) -= t_2;
        }
      });
    }
  }

  template <typename State> void periodic(State&& s) {
    using l = typename std::decay_t<State>::lattice_t;
    for (auto&& b : s.blocks()) {
      // periodic on inflow/outflow
      auto offset = b.internal_cells_per_length();
      b.for_each_internal([&](auto c) {
        if (c.x[0] == 2) {
          RANGES_FOR (auto&& d, l::node_ids()) {
            b.nodes1(c, d) = b.nodes0(c.idx + offset - 1, d);
          }
        }
        if (c.x[0] == 101) {
          RANGES_FOR (auto&& d, l::node_ids()) {
            b.nodes1(c, d) = b.nodes0(c.idx - (offset - 1), d);
          }
        }
      });
    }
  }

  template <typename State> void bounce_back(State&& s) {
    using l = typename std::decay_t<State>::lattice_t;
    for (auto&& b : s.blocks()) {
      // bounce back on solid boundaries
      b.for_each_internal([&](auto c) {
        if (c.x[0] == 0 || c.x[0] == 1 || c.x[1] == 2 || c.x[1] == 101
            || c.x[1] == 102 || c.x[1] == 103 || (*this)(b.center(c)) < 0.) {
          RANGES_FOR (auto&& d, l::node_ids()) {
            b.nodes0(c, d) = b.nodes1(c, l::opposite_dist(d));
          }
        }
      });
    }
  }
};

int main() {
  mpi::env env;
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("lbm_d2q9", comm);
  io::session s(io::create, "lbm_d2q9", comm);

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

  using lbm_state = solver::lbm::state<solver::lbm::lattice::d2q9_2>;

  lbm_state lbm_s{g, 0_g, solver_block_capacity};
  num_t density = 0.1;
  num_t accel   = 0.015;
  BoundaryConditions bcs(density, accel);

  /// Initial lbm solver grid:
  solver::initialize_grid(g, lbm_s, [&](auto&& n) { return g.is_leaf(n); });

  lbm::init_variables(lbm_s, density);

  lbm::vtk::ls_serialize(lbm_s, bcs, "lbm2_res", 0);

  lbm::advance(lbm_s, bcs, 100);

  return 0;
}
