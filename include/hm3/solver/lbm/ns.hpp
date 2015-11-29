#pragma once
/// \file
#include <hm3/solver/lbm/ns/ns.hpp>

namespace hm3 {
namespace solver {
namespace lbm {
namespace ns {

template <typename Lattice>  //
struct physics {
  using l = Lattice;

  static constexpr num_t cs_2() noexcept { return l::cs() * l::cs(); }

  num_t mach     = 0.1;
  num_t reynolds = 100;
  num_t length   = 1.0;

  static constexpr num_t nu(num_t mach, num_t reynolds, num_t length,
                            num_t max_level, num_t level) noexcept {
    return mach / l::cs() / reynolds * length / std::pow(max_level - level, 2.);
  }

  constexpr num_t nu(num_t max_level, num_t level) const noexcept {
    return nu(max, reynolds, length, max_level, level);
  }

  static constexpr num_t omega(num_t nu) noexcept {
    return 2.0 / (1.0 + 6.0 * nu);
  }

  constexpr num_t omega(max_level, level) const noexcept {
    return omega(nu(max_level, level));
  }

  template <typename Distributions>
  static constexpr num_t rho(Distributions&& d) noexcept {
    num_t tmp = 0.;
    RANGES_FOR (auto&& d_i, l::all()) { tmp += d[d_i]; }
    return tmp;
  }

  static constexpr num_t p(num_t density) noexcept { return density * cs_2(); }

  template <typename Distributions,
            CONCEPT_REQUIRES_(!Same<num_t, std::decay_t<Distributions>>{})>
  constexpr num_t p(Distributions&& d) const noexcept {
    return p(rho(d), cs_2());
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t u(Distributions&& d, num_t density) noexcept {
    return (d[l::E] + d[l::NE] + d[l::SE] - (d[l::W] + d[l::NW] + d[l::SW]))
           / density;
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static constexpr num_t v(Distributions&& d, num_t density) noexcept {
    return (d[l::N] + d[l::NW] + d[l::NE] - (d[l::S] + d[l::SW] + d[l::SE]))
           / density;
  }

  template <typename Distributions, CONCEPT_REQUIRES_(l::dimension() == 2)>
  static num_a<2> us(Distributions&& d, num_t density) noexcept {
    num_a<2> tmp{{u(d, density), v(d, density)}};
    return tmp;
  }

  template <typename Distributions>
  static num_a<l::dimension()> us(Distributions&& d) noexcept {
    return us(d, rho(d));
  }

  template <typename State, typename CellData>
  static void load(State&& s, CellData&& cd) noexcept {
    cd.load("rho", [&](auto c, auto&&) { return rho(&(s.nodes0(c, 0))); });
    cd.load("p",
            [&](auto c, auto&&) { return p(rho(&(s.nodes0(c, 0)), c_squ()); });

    cd.load("u", [&](auto c, auto&&) {
      return u(&(s.nodes0(c, 0)), rho(&(s.nodes0(c, 0))));
    });
    cd.load("v", [&](auto c, auto&&) {
      return v(&(s.nodes0(c, 0)), rho(&(s.nodes0(c, 0))));
    });
  }

  template <typename Distributions, typename Lattice>
  constexpr std::array<num_t, 9> equilibrium(Distributions&& d) {
    std::array<num_t, 9> f_eq;
    // integral local density:
    const num_t rho = rho(from);
    // x-, and y- velocity components:
    const auto us = us(from, rho);

    const num_t cs_2    = cs_2();
    const num_t inv_cs2 = 1. / cs_2;

    const num_t u0_2 = us[0] * us[0];
    const num_t u1_2 = us[1] * us[1];
    const num_t c0   = 1. / 9. * inv_cs2;
    const num_t c1   = rho * cs_2;
    const num_t c2   = 1. / 36. * inv_cs2;
    const num_t c3   = 4. / 9. * inv_cs2;
    const num_t f0_0 = .5 * u0_2 * (inv_cs2 - 1.) - .5 * u1_2;
    const num_t f0_1 = .5 * u1_2 * (inv_cs2 - 1.) - .5 * u0_2;
    const num_t f1   = .5 * (u0_2 + u1_2) * (inv_cs2 - 1);
    const num_t f2   = inv_cs2 * us[0] * us[1];

    f_eq[l::W]  = c0 * (c1 - us[0] + f0_0);
    f_eq[l::E]  = c0 * (c1 + us[0] + f0_0);
    f_eq[l::S]  = c0 * (c1 - us[1] + f0_1);
    f_eq[l::N]  = c0 * (c1 + us[1] + f0_1);
    f_eq[l::NE] = c2 * (c1 + us[0] + us[1] + f1 + f2);
    f_eq[l::SE] = c2 * (c1 + us[0] - us[1] + f1 - f2);
    f_eq[l::SW] = c2 * (c1 - us[0] - us[1] + f1 + f2);
    f_eq[l::NW] = c2 * (c1 - us[0] + us[1] + f1 - f2);
    f_eq[l::C] = c3 * (c1 - .5 * (u0_2 + u1_2));
    return f_eq;
  }
};

// One-step collision
template <typename Distributions>
static constexpr void collide(Distributions&& from, Distributions&& to,
                              navier_stokes<d2q9_2> physics) noexcept {
  num_t omega = physics.omega(max_level, level);

  auto f_eq = equilibrium(from, physics);

  RANGES_FOR (auto&& d, l::all()) {
    to[d] = from[d] + omega * (f_eq[d] - from[d]);
  }
}

}  // namespace ns
}  // namespace lbm
}  // namespace solver
}  // namespace hm3
