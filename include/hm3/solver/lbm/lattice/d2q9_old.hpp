#pragma once
/// \file
#include <hm3/geometry/dimensions.hpp>
#include <hm3/tree/relations/neighbor.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace lbm {
namespace lattice {

/// D2Q9 Lattice:
///
///  6   2   5
///
///  3   0   1
///
///  7   4   8
///
struct d2q9 : geometry::dimensional<2> {
  static constexpr suint_t no_distributions() noexcept { return 9; }
  static auto distributions() noexcept {
    return view::iota(0, no_distributions());
  }
  static auto center_node_id() noexcept { return 0; }
  static auto node_ids() noexcept { return view::iota(0, 9); }
  static auto node_ids_without_center() noexcept { return view::iota(1, 9); }
  static auto direct_node_ids() noexcept { return view::iota(1, 5); }
  static auto diagonal_node_ids() noexcept { return view::iota(5, 9); }

  /// go from lattice neighbors to tree neighbors and use that to compute the
  /// offset
  ///
  /// TODO: clean this up, maybe reorder the lattice to match the tree
  /// neighbors?
  static auto neighbor_offset(suint_t d) noexcept {
    HM3_ASSERT(d > 0, "distribution 0 has no neighbor offset");
    d -= 1;
    suint_t d_ts[] = {1, 3, 0, 2, 3, 2, 0, 1};
    suint_t d_t    = d_ts[d];
    using namespace tree;
    return (d < 4) ? face_neighbors<2>()[d_t] : edge_neighbors<2>()[d_t];
  }
  static suint_t opposite_dist(suint_t d) noexcept {
    suint_t od[9] = {0, 3, 4, 1, 2, 7, 8, 5, 6};
    return od[d];
  }

  template <typename Distributions>
  static constexpr num_t rho(Distributions&& d) noexcept {
    num_t tmp = 0;
    RANGES_FOR (auto&& d_i, distributions()) { tmp += d[d_i]; }
    return tmp;
  }

  static constexpr num_t p(num_t density, num_t c_squ) noexcept {
    return density * c_squ;
  }

  template <typename Distributions>
  static constexpr num_t u(Distributions&& d, num_t density) noexcept {
    return (d[1] + d[5] + d[8] - (d[3] + d[6] + d[7])) / density;
  }

  template <typename Distributions>
  static constexpr num_t v(Distributions&& d, num_t density) noexcept {
    return (d[2] + d[5] + d[6] - (d[4] + d[7] + d[8])) / density;
  }

  template <typename Distributions>
  static num_a<2> velocities(Distributions&& d, num_t density) noexcept {
    num_a<2> tmp;
    tmp(0) = u(d, density);
    tmp(1) = v(d, density);
    return tmp;
  }

  template <typename Distributions>
  static num_a<2> velocities(Distributions&& d) noexcept {
    return velocities(d, density(d));
  }

  template <typename State, typename CellData>
  static void load(State&& s, CellData&& cd) noexcept {
    cd.load("rho", [&](auto c, auto&&) { return rho(&(s.nodes0(c, 0))); });
    cd.load("u", [&](auto c, auto&&) {
      return u(&(s.nodes0(c, 0)), rho(&(s.nodes0(c, 0))));
    });
    cd.load("v", [&](auto c, auto&&) {
      return v(&(s.nodes0(c, 0)), rho(&(s.nodes0(c, 0))));
    });
  }

  // One-step collision
  template <typename Distributions>
  static constexpr void collide(Distributions&& from, Distributions&& to,
                                num_t c_squ, num_t omega) {
    // weighting factors (depending on lattice geometry)
    constexpr num_t t_0 = 4. / 9.;
    constexpr num_t t_1 = 1. / 9.;
    constexpr num_t t_2 = 1. / 36.;

    // integral local density:
    const num_t dloc = rho(from);

    // x-, and y- velocity components:
    const num_t u_x = u(from, dloc);
    const num_t u_y = v(from, dloc);

    // n- velocity components (n = lattice node connection vectors)
    // this is only necessary for clearence, and only 3 speeds would
    // be necessary
    const num_t sx[9] = {0., 1., 0., -1., 0., 1., -1., -1., 1.};
    const num_t sy[9] = {0., 0., 1., 0., -1., 1., 1., -1., -1.};
    num_t u_n[9]      = {0};
    RANGES_FOR (auto i, distributions()) { u_n[i] = sx[i] * u_x + sy[i] * u_y; }

    // equilibrium densities:
    // (this can be rewritten to improve computational performance
    // considerabely !)
    const num_t f1    = (2. * c_squ);
    const num_t f0    = f1 * c_squ;
    const num_t u_squ = u_x * u_x + u_y * u_y;  // square velocity
    const num_t f2    = u_squ / f1;
    const num_t f3    = t_1 * dloc;
    const num_t f4    = t_2 * dloc;

    num_t n_equ[9] = {0};
    n_equ[0]       = t_0 * dloc * (1. - f2);  // zero velocity density

    // axis speeds (factor: t_1)
    RANGES_FOR (auto i, direct_node_ids()) {
      n_equ[i] = f3 * (1. + u_n[i] / c_squ + std::pow(u_n[i], 2.) / f0 - f2);
    }

    // diagonal speeds (factor: t_2)
    RANGES_FOR (auto i, diagonal_node_ids()) {
      n_equ[i] = f4 * (1. + u_n[i] / c_squ + std::pow(u_n[i], 2.) / f0 - f2);
    }

    // relaxation step
    RANGES_FOR (auto n, node_ids()) {
      to[n] = from[n] + omega * (n_equ[n] - from[n]);
    }
  }
};

}  // namespace lattice
}  // namespace lbm
}  // namespace solver
}  // namespace hm3
