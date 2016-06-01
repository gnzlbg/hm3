#pragma once
/// \file
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace solver {
namespace lbm {
namespace ns {

template <typename Lattice>  //
struct physics : Lattice {
  using typename Lattice::indices;
  using l = Lattice;

  using vel_t  = array<num_t, l::dimension()>;
  using dist_t = array<num_t, l::size()>;

  struct variables {
    num_t rho;
    vel_t us;
  };

  num_t mach            = 0.1;
  num_t reynolds        = 100;
  num_t min_cell_length = 1.0;

  static constexpr num_t nu(num_t mach, num_t reynolds,
                            num_t min_cell_length) noexcept {
    return mach * l::cs() / reynolds * min_cell_length;
  }
  constexpr num_t nu() const noexcept {
    return nu(mach, reynolds, min_cell_length);
  }

  static constexpr num_t omega(num_t nu, num_t cell_length) noexcept {
    return 2.0 / (1.0 + 6.0 * nu / cell_length);
  }

  constexpr num_t omega(num_t cell_length) const noexcept {
    return omega(nu(), cell_length);
  }

  template <typename Distributions,
            CONCEPT_REQUIRES_(!Same<num_t, std::decay_t<Distributions>>{})>
  static constexpr num_t rho(Distributions&& d) noexcept {
    num_t tmp = 0.;
    RANGES_FOR (auto&& d_i, l::all()) { tmp += d[d_i]; }
    return tmp;
  }

  static constexpr num_t rho(num_t p) noexcept {
    return p / (l::cs() * l::cs());
  }

  static constexpr num_t p(num_t density) noexcept {
    return density * l::cs() * l::cs();
  }

  template <typename Distributions,
            CONCEPT_REQUIRES_(!Same<num_t, uncvref_t<Distributions>>{})>
  static constexpr num_t p(Distributions&& d) noexcept {
    return p(rho(d));
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t u(Distributions&& d, num_t density) noexcept {
    return (d[l::e] + d[l::ne] + d[l::se] - (d[l::w] + d[l::nw] + d[l::sw]))
           / density;
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t u(Distributions&& d) noexcept {
    return u(d, rho(d));
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t v(Distributions&& d, num_t density) noexcept {
    return (d[l::n] + d[l::nw] + d[l::ne] - (d[l::s] + d[l::sw] + d[l::se]))
           / density;
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t v(Distributions&& d) noexcept {
    return v(d, rho(d));
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static vel_t us(Distributions&& d, num_t density) noexcept {
    vel_t tmp{{u(d, density), v(d, density)}};
    return tmp;
  }

  template <typename Distributions>
  static vel_t us(Distributions&& d) noexcept {
    return us(d, rho(d));
  }

  template <typename State, typename CellData>
  static void load(State&& s, CellData&& cd) noexcept {
    cd.load("rho0", [&](auto c, auto&&) { return rho(&(s.nodes0(c, 0))); });
    cd.load("p0", [&](auto c, auto&&) { return p(rho(&(s.nodes0(c, 0)))); });
    cd.load("u0", [&](auto c, auto d) { return us(&(s.nodes0(c, 0)))[d]; },
            s.dimension(), [&](auto&& d) { return s.dimension_name(d); });
    cd.load("rho1", [&](auto c, auto&&) { return rho(&(s.nodes1(c, 0))); });
    cd.load("p1", [&](auto c, auto&&) { return p(rho(&(s.nodes1(c, 0)))); });
    cd.load("u1", [&](auto c, auto d) { return us(&(s.nodes1(c, 0)))[d]; },
            s.dimension(), [&](auto&& d) { return s.dimension_name(d); });
  }

  static constexpr num_t f1() noexcept { return 1. / (l::cs() * l::cs()); }
  static constexpr num_t f2() noexcept { return 0.5 * f1() * f1(); }

  static constexpr dist_t equilibrium_distribution(num_t rho,
                                                   vel_t us) noexcept {
    dist_t f_eq = {{0}};

    num_t u_2 = 0.0;
    RANGES_FOR (auto&& dim, l::dimensions()) { u_2 += us[dim] * us[dim]; }

    const num_t f3 = 0.5 * u_2 * f1();

    RANGES_FOR (auto&& d, l::all()) {
      num_t ei_u = 0.0;
      RANGES_FOR (auto&& dim, l::dimensions()) {
        ei_u += l::dir(d)[dim] * us[dim];
      }
      const num_t ei_u_2 = ei_u * ei_u;
      num_t omega_i      = l::constants(d);
      f_eq[d] = rho * omega_i * (1. + ei_u * f1() + (ei_u_2)*f2() - f3);
    }
    return f_eq;
  }

  static constexpr dist_t equilibrium_distribution(variables v) {
    return equilibrium_distribution(v.rho, v.us);
  }

  template <typename Distributions,
            CONCEPT_REQUIRES_(!Same<std::decay_t<Distributions>, variables>{})>
  static constexpr dist_t equilibrium_distribution(
   Distributions&& distributions) {
    // compute density and velocity components:
    const num_t rho_ = rho(distributions);
    const auto us_   = us(distributions, rho_);
    return equilibrium_distribution(rho_, us_);
  }

  template <typename Distributions,
            CONCEPT_REQUIRES_(!Same<std::decay_t<Distributions>, variables>{})>
  static constexpr dist_t non_equilibrium_distribution(
   Distributions&& distributions) {
    auto eq = equilibrium_distribution(distributions);
    RANGES_FOR (auto&& d, l::all()) {
      eq[d] = distributions[d] - eq[d];  //: f_non_eq = f - f_eq
    }
    return eq;
  }

  template <typename Distributions>
  static constexpr dist_t bounce_back(Distributions&&) {}
};

}  // namespace ns
}  // namespace lbm
}  // namespace solver
}  // namespace hm3
