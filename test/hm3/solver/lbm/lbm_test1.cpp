#ifndef LBM_HPP_
#define LBM_HPP_
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <cmath>
#include <cstdio>
#include <functional>
#include <hm3/solver/lbm/lattice/d2q9.hpp>
#include <hm3/solver/lbm/ns.hpp>
#include <range/v3/all.hpp>
#include <vector>
////////////////////////////////////////////////////////////////////////////////

namespace hm3 {

namespace lbm {

using lattice = solver::lbm::lattice::d2q9;
namespace ns  = solver::lbm::ns;

template <class I> using Range = std::decay_t<decltype(view::iota(I{0}, I{4}))>;
// thrust::iterator_range<thrust::counting_iterator<I>>;

struct cylinder {
  cylinder(const idx_t lx, const idx_t ly)
   : x_c(lx / 2. - 0.2 * lx), y_c(ly / 2.), r(0.125 * ly) {}
  const num_t x_c;
  const num_t y_c;
  const num_t r;
};

struct node_vars_t {};
static const constexpr node_vars_t node_vars{};
struct hlp_vars_t {};
static const constexpr hlp_vars_t hlp_vars{};

struct data_structure {
  data_structure(const idx_t size_x, const idx_t size_y)
   : lx(size_x)
   , ly(size_y)
   , cyl(lx, ly)
   , nodes_(no_nodes())
   , nodes_hlp_(no_nodes()) {
    init();
  }

  idx_t n_size() const { return 9; }
  idx_t x_size() const { return lx; }
  idx_t y_size() const { return ly; }

  idx_t no_cells() const { return x_size() * y_size(); }
  idx_t no_nodes() const { return n_size() * no_cells(); }

  auto cell_ids() const { return view::iota(idx_t{0}, no_cells()); }
  auto node_ids() const { return lattice::all(); }

  idx_t cell_at(const idx_t xidx_t, const idx_t yidx_t) const {
    return xidx_t + x_size() * yidx_t;
  }

  idx_t neighbor(const idx_t c, const idx_t n) const {
    auto x_i = x(c);
    auto y_i = y(c);
    // compute upper and right next neighbour nodes with regard to periodic
    // boundaries:
    const int y_n = (y_i + 1) % ly + 1 - 1;  // todo: recheck!
    const int x_e = (x_i + 1) % lx + 1 - 1;  // todo: recheck!
    // compute lower and left next neighbour nodes with regard to periodic
    // boundaries:
    const int y_s = ly - (ly + 1 - (y_i + 1)) % ly - 1;
    const int x_w = lx - (lx + 1 - (x_i + 1)) % lx - 1;

    const hm3::array<idx_t, 9> nghbrs
     = {{c, cell_at(x_e, y_i), cell_at(x_i, y_n), cell_at(x_w, y_i),
         cell_at(x_i, y_s), cell_at(x_e, y_n), cell_at(x_w, y_n),
         cell_at(x_w, y_s), cell_at(x_e, y_s)}};

    return nghbrs[n];
  }

  num_t* vars(node_vars_t, const idx_t cidx_t) { return &nodes(cidx_t, 0); }
  num_t const* vars(node_vars_t, const idx_t cidx_t) const {
    return &nodes(cidx_t, 0);
  }

  num_t* vars(hlp_vars_t, const idx_t cidx_t) { return &nodes_hlp(cidx_t, 0); }
  num_t const* vars(hlp_vars_t, const idx_t cidx_t) const {
    return &nodes_hlp(cidx_t, 0);
  }

  num_t& vars(node_vars_t, const idx_t cidx_t, const idx_t nidx_t) {
    return nodes(cidx_t, nidx_t);
  }

  num_t const& vars(node_vars_t, const idx_t cidx_t, const idx_t nidx_t) const {
    return nodes(cidx_t, nidx_t);
  }

  num_t& vars(hlp_vars_t, const idx_t cidx_t, const idx_t nidx_t) {
    return nodes_hlp(cidx_t, nidx_t);
  }

  num_t const& vars(hlp_vars_t, const idx_t cidx_t, const idx_t nidx_t) const {
    return nodes_hlp(cidx_t, nidx_t);
  }

  num_t& nodes(const idx_t cidx_t, const idx_t nidx_t) {
    return nodes_[cidx_t * n_size() + nidx_t];
  }

  num_t const& nodes(const idx_t cidx_t, const idx_t nidx_t) const {
    return nodes_[cidx_t * n_size() + nidx_t];
  }

  num_t& nodes_hlp(const idx_t cidx_t, const idx_t nidx_t) {
    return nodes_hlp_[cidx_t * n_size() + nidx_t];
  }

  num_t const& nodes_hlp(const idx_t cidx_t, const idx_t nidx_t) const {
    return nodes_hlp_[cidx_t * n_size() + nidx_t];
  }

  num_t& nodes(const idx_t nidx_t, const idx_t xidx_t, const idx_t yidx_t) {
    return nodes_[idx(nidx_t, xidx_t, yidx_t)];
  }

  num_t& nodes_hlp(const idx_t nidx_t, const idx_t xidx_t, const idx_t yidx_t) {
    return nodes_hlp_[idx(nidx_t, xidx_t, yidx_t)];
  }

  idx_t x(idx_t cidx_t) const {
    while (cidx_t > lx - 1) { cidx_t -= lx; }
    return cidx_t;
  }
  int y(const idx_t cidx_t) const { return cidx_t / lx; }

  bool obst(const idx_t x, const idx_t y) const {
    return in_cylinder(x, y) || y == 0 || y == ly - 1;
  }

  bool obst(const idx_t cidx_t) const { return obst(x(cidx_t), y(cidx_t)); }

  template <class T> num_t d_loc(T, const idx_t cidx_t) const {
    return ns::physics<lattice>::rho(vars(T{}, cidx_t));
  }

  template <class T> num_t p(T, const idx_t cidx_t) const {
    if (obst(cidx_t)) {
      return ns::physics<lattice>::p(density);
    } else {
      return ns::physics<lattice>::p(vars(T{}, cidx_t));
    }
  }

  template <class T> std::vector<num_t> ps(T) const {
    std::vector<num_t> ps_(no_cells());
    transform(cell_ids(), ps_.begin(), [&](auto i) { return p(T{}, i); });
    return ps_;
  }

  template <class T> num_t u(T, const idx_t cidx_t) const {
    if (obst(cidx_t)) {
      return 0.;
    } else {
      return ns::physics<lattice>::u(vars(T{}, cidx_t));
    }
  }

  template <class T> std::vector<num_t> us(T) const {
    std::vector<num_t> ps_(no_cells());
    transform(cell_ids(), ps_.begin(), [&](auto i) { return u(T{}, i); });
    return ps_;
  }

  template <class T> num_t v(T, const idx_t cidx_t) const {
    if (obst(cidx_t)) {
      return 0.;
    } else {
      return ns::physics<lattice>::v(vars(T{}, cidx_t));
    }
  }

  template <class T> std::vector<num_t> vs(T) const {
    std::vector<num_t> ps_(no_cells());
    transform(cell_ids(), ps_.begin(), [&](auto i) { return v(T{}, i); });
    return ps_;
  }

  // Initialize density distribution function n with equilibrium
  // for zero velocity
  void init() {
    auto f_eq
     = ns::physics<lattice>::equilibrium_distribution(density, {{0.0, 0.0}});
    for_each(cell_ids(), [&](auto c) {
      for (auto i : lattice::all()) { nodes(c, i) = f_eq[i]; }
    });
  }

  // Compute integral density (should remain constant)
  void check_density(int time_it) const {
    double n_sum = accumulate(
     cell_ids() | view::transform([&](auto c) { return d_loc(node_vars, c); }),
     num_t{0});
    printf("# %d | integral density: %8.f\n", time_it, n_sum);
  }

  // One-step density relaxation process
  void relaxation() {
    for_each(cell_ids(), [&](auto c) {
      if (obst(c)) { return; }

      // integral local density:
      const num_t dloc = d_loc(hlp_vars, c);

      // x-, and y- velocity components:
      const num_t u_x = u(hlp_vars, c);
      const num_t u_y = v(hlp_vars, c);

      const auto f_eq
       = ns::physics<lattice>::equilibrium_distribution(dloc, {{u_x, u_y}});

      // relaxation step
      for (auto n : node_ids()) {
        nodes(c, n) = nodes_hlp(c, n) + omega * (f_eq[n] - nodes_hlp(c, n));
      }
    });
  }

  // Fluid densities are rotated by the next propagation step, this results in a
  // bounce back from cells.obstacle nodes.
  // solid-wall condition?
  void bounceback() {
    for_each(cell_ids(), [&](auto c) {
      if (obst(c)) {
        for (auto n : node_ids()) {
          nodes(c, n) = nodes_hlp(c, lattice::opposite(n));
        }
      }
    });
  }

  // Propagate fluid densities to their next neighbour nodes
  // streaming operator?
  void propagate() {
    for_each(cell_ids(), [&](auto c) {
      for (auto n : node_ids()) { nodes_hlp(neighbor(c, n), n) = nodes(c, n); }
    });
  }

  // density redistribution in first lattice column
  // inlet boundary condition?
  void redistribute() {
    // compute weighting factors (depending on lattice geometry) for
    // increasing/decreasing inlet densities
    const num_t t_1 = density * accel / 9.;
    const num_t t_2 = density * accel / 36.;

    for_each(cell_ids(), [&](auto c) {
      auto xi = x(c);
      if (xi != 0) { return; }
      auto yi = y(c);
      if (!obst(0, yi)  // accelerate flow only on non-occupied nodes
          // check to avoid negative densities:
          && nodes(3, 0, yi) - t_1 > 0. && nodes(6, 0, yi) - t_2 > 0.
          && nodes(7, 0, yi) - t_2 > 0.) {
        nodes(1, 0, yi) += t_1;  // increase east
        nodes(5, 0, yi) += t_2;  // increase north-east
        nodes(8, 0, yi) += t_2;  // increase south-east

        nodes(3, 0, yi) -= t_1;  // decrease west
        nodes(6, 0, yi) -= t_2;  // decrease north-west
        nodes(7, 0, yi) -= t_2;  // decrease south-west
      }
    });
  }

 private:
  idx_t idx(const idx_t nidx_t, const idx_t xidx_t, const idx_t yidx_t) {
    return nidx_t + n_size() * (xidx_t + x_size() * yidx_t);
  }

  bool in_cylinder(const idx_t xi, const idx_t yi) const {
    return std::sqrt(std::pow(cyl.x_c - xi, 2.) + std::pow(cyl.y_c - yi, 2))
            - cyl.r
           < 0.;
  }

  const idx_t lx;
  const idx_t ly;
  const cylinder cyl;

  // fluid densities: a 9-speed lattice is used here, other geometries are
  // possible the densities are numbered as follows:
  //
  //              6   2   5
  //                \ | /
  //              3 - 0 - 1
  //                / |                         \
  //              7   4   8
  // the lattice nodes are numbered as follows:
  //
  //     ^
  //     |
  //     y
  //
  //        :    :    :
  //
  //   3    *    *    *  ..
  //
  //   2    *    *    *  ..
  //
  //   1    *    *    *  ..
  //                         x ->
  //        1    2    3
  std::vector<num_t> nodes_;
  std::vector<num_t> nodes_hlp_;

  const num_t density = 0.1;   // fluid density per link
  const num_t omega   = 1.85;  // relaxation parameter
  const num_t accel   = 0.;    // 0.015;    // accelleration
};

void write_vtk(const data_structure& cells, int time_it) {
  char fname[100];
  sprintf(fname, "output_%d.vtk", time_it);

  FILE* f = fopen(fname, "w");
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "LBM test output\n");
  fprintf(f, "ASCII\n");
  fprintf(f, "DATASET UNSTRUCTURED_GRID\n");

  num_t x_stencil[4] = {-1, 1, -1, 1};
  num_t y_stencil[4] = {-1, -1, 1, 1};
  num_t length       = 1;
  fprintf(f, "POINTS %ld FLOAT\n", cells.no_cells() * 4);
  for (auto i : cells.cell_ids()) {
    num_t x = static_cast<num_t>(cells.x(i));
    num_t y = static_cast<num_t>(cells.y(i));
    for (int j = 0; j < 4; ++j) {
      num_t xp = x + x_stencil[j] * length / 2;
      num_t yp = y + y_stencil[j] * length / 2;
      fprintf(f, "%f %f 0.0\n", xp, yp);
    }
  }

  fprintf(f, "CELLS %ld %ld\n", cells.no_cells(), cells.no_cells() * 5);
  for (auto i : cells.cell_ids()) {
    fprintf(f, "4 %ld %ld %ld %ld\n", 4 * i, 4 * i + 1, 4 * i + 2, 4 * i + 3);
  }

  fprintf(f, "CELL_TYPES %ld\n", cells.no_cells());
  for_each(cells.cell_ids(), [&](auto) { fprintf(f, "8\n"); });

  fprintf(f, "CELL_DATA %ld\n", cells.no_cells());

  fprintf(f, "SCALARS p float\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    auto ps = cells.ps(node_vars);
    for (auto i : cells.cell_ids()) { fprintf(f, "%f\n", ps[i]); }
  }

  fprintf(f, "SCALARS u float\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    auto us = cells.us(node_vars);
    for (auto i : cells.cell_ids()) { fprintf(f, "%f\n", us[i]); }
  }

  fprintf(f, "SCALARS v float\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    auto vs = cells.vs(node_vars);
    for (auto i : cells.cell_ids()) { fprintf(f, "%f\n", vs[i]); }
  }

  fprintf(f, "SCALARS active int\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  for (auto i : cells.cell_ids()) { fprintf(f, "%d\n", !cells.obst(i)); }

  fclose(f);
}

}  // namespace lbm

}  // namespace hm3

////////////////////////////////////////////////////////////////////////////////
#endif  // LBM_HPP_

using namespace hm3;

// #include "lbm.hpp"
// #include "tbb/task_scheduler_init.h"

// grid size in x- and y-dimension
static const int lx = 100;
static const int ly = 100;

static const int t_max = 20;  // maximum number of iterations
int time_it            = 0;   // iteration counter

lbm::data_structure cells(lx, ly);

// void write_distributions(bool stop = false) {
//   printf("# WRITING DISTRIBUTIONS AT TIME=%d\n", time_it);
//   for (auto c: cells.cell_ids()) {
//     for (auto n : cells.node_ids()) {
//       printf("x=%ld \t y=%d \t n=%ld \t f=%e \t fh=%e \n", cells.x(c),
//              cells.y(c), n, cells.nodes(c, n), cells.nodes_hlp(c, n));
//     }
//   }
//   if (stop) { std::terminate(); }
// }

int main() {
  while (time_it < t_max) {
    ++time_it;
    // if (time_it % 200 == 0) {
    cells.check_density(time_it);
    //}
    cells.redistribute();
    cells.propagate();
    cells.bounceback();
    cells.relaxation();
    // if(time_it > 16540 && time_it % 10 == 0) {
    write_vtk(cells, time_it);
    //}
  }

  return 0;
}

/*
#include <hm3/geometry/intersect.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/grid/hierarchical/generation/uniform.hpp>
#include <hm3/io/session.hpp>
#include <hm3/solver/lbm/lattice/d2q9.hpp>
#include <hm3/solver/lbm/lbm.hpp>
#include <hm3/solver/lbm/state.hpp>
#include <hm3/solver/lbm/vtk.hpp>
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

  using lbm_state = solver::lbm::state<solver::lbm::lattice::d2q9>;

  lbm_state lbm_s{g, 0_g, solver_block_capacity};
  num_t density = 0.1;
  num_t accel   = 0.015;
  BoundaryConditions bcs(density, accel);

  /// Initial lbm solver grid:
  solver::initialize_grid(g, lbm_s, [&](auto&& n) { return g.is_leaf(n); });

  lbm::init_variables(lbm_s, density);

  lbm::vtk::ls_serialize(lbm_s, bcs, "lbm_res", 0);

  lbm::advance(lbm_s, bcs, 100);

  return 0;
}
*/
