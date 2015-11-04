#pragma once
/// \file
///
///
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct AUSM {};
struct LxF {};

template <uint_t Nd, typename Flux = AUSM> struct euler : dimensional<Nd> {
  /// \name Physical constants
  ///@{
  num_t gamma;
  num_t gamma_m1;
  ///@}

  /// \name Indices
  ///@{

  using dimensional<Nd>::dimension;
  using dimensional<Nd>::dimensions;
  using dimensional<Nd>::dimension_name;

  static constexpr uint_t no_variables() noexcept { return dimension() + 2; }
  static constexpr auto variables() noexcept {
    return view::iota(0_u, no_variables());
  }

  /// \name Conservative variables
  ///@{
  static constexpr sidx_t rho_u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t rho() noexcept { return dimension(); }
  static constexpr sidx_t rho_E() noexcept { return dimension() + 1; }
  ///@}  // Convervative variables

  /// \name Primitive variables
  ///@{

  // Note: primitive rho() == convervative rho()

  static constexpr sidx_t u(sidx_t d) noexcept { return d; }
  static constexpr sidx_t p() noexcept { return dimension() + 1; }

  ///@}  // Primitive variables

  ///@} // Indices

  euler(num_t gamma_) : gamma{gamma_}, gamma_m1{gamma_ - 1.0} {
    HM3_ASSERT(gamma > 0., "negative gamma");
    HM3_ASSERT(gamma_m1 > 0., "negative gamma-1.0");
  }

  /// \name Physical relations
  ///@{

  template <typename PV> static constexpr decltype(auto) vels(PV&& v) noexcept {
    return v.template head<dimension()>();
  }

  template <typename CV>
  static constexpr decltype(auto) rho_us(CV&& v) noexcept {
    return v.template head<dimension()>();
  }

  template <typename CV>
  static constexpr num_t velocity(CV&& cv, uint_t d) noexcept {
    //HM3_ASSERT(cv(rho()) > 0., "negative density!");
    return cv(rho_u(d)) / cv(rho());
  }

  template <typename CV>
  static constexpr decltype(auto) velocities(CV&& cv) noexcept {
    // HM3_ASSERT(cv(rho()) > 0., "negative density!");
    return rho_us(cv) / cv(rho());
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2} from the conservative
  /// variables
  template <typename CV> static constexpr num_t u_mag2_cv(CV&& v) noexcept {
    HM3_ASSERT(static_cast<bool>(v(rho()) > 0.), "negative density!");
    num_t r = rho_us(v).squaredNorm() / (v(rho()) * v(rho()));
    return r;
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2} from the primitive
  /// variables
  template <typename PV> static constexpr num_t u_mag2_pv(PV&& v) noexcept {
    num_t r = vels(v).squaredNorm();
    return r;
  }

  /// Pressure
  ///
  /// $ p = (\gamma - 1) \rho (E - ||u||_{L2}^2 / 2) $
  static constexpr num_t pressure(num_t gamma_m1, num_t u_mag2, num_t rho,
                                  num_t rho_E) noexcept {
    return gamma_m1 * (rho_E - 0.5 * rho * u_mag2);
  }

  /// Pressure from the conservative variables
  template <typename CV>
  static constexpr num_t pressure(num_t gamma_m1, CV&& v) noexcept {
    return pressure(gamma_m1, u_mag2_cv(v), v(rho()), v(rho_E()));
  }

  /// Pressure from the conservative variables
  template <typename CV> constexpr num_t pressure(CV&& v) const noexcept {
    return pressure(gamma_m1, std::forward<CV>(v));
  }

  /// Energy density
  ///
  /// $\rho E = p / (\gamma - 1) + \rho ||u||_{L2}^2 / 2 $
  static constexpr num_t energy_density(num_t gamma_m1, num_t u_mag2, num_t rho,
                                        num_t pressure) noexcept {
    return pressure / gamma_m1 + 0.5 * rho * u_mag2;
  }

  /// Energy density from the primitive variables
  template <typename PV>
  static constexpr num_t energy_density(num_t gamma_m1, PV&& pv) noexcept {
    return energy_density(gamma_m1, u_mag2_pv(pv), pv(rho()), pv(p()));
  }

  /// Energy density from the primitive variables
  template <typename PV> constexpr num_t energy_density(PV&& pv) noexcept {
    return energy_density(gamma_m1, std::forward<PV>(pv));
  }

  /// Inplace conversion of primitive to conservative variables
  template <typename PV>
  static constexpr void pv_to_cv(PV&& v, num_t gamma_m1) noexcept {
    auto umag2 = u_mag2_pv(v);
    // rho = rho (no op)
    num_t r = v(rho());
    // u * rho = rho u
    vels(v) *= r;
    // rho_e = p / (\gamma - 1) + \rho ||u||_2^2 / 2
    v(rho_E()) = energy_density(gamma_m1, umag2, r, v(p()));
  }

  /// Inplace conversion of primitive to conservative variables
  template <typename PV> constexpr void pv_to_cv(PV&& v) const noexcept {
    pv_to_cv(std::forward<PV>(v), gamma_m1);
  }

  /// Inplace conversion of conservative to primitive variables
  template <typename CV>
  static constexpr void cv_to_pv(CV&& v, num_t gamma_m1) noexcept {
    // rho = rho (no op)
    num_t r = v(rho());
    // rho_u / rho = u
    vels(v) /= r;
    auto umag2 = u_mag2_pv(v);
    num_t tmp = gamma_m1 * (v(rho_E()) - 0.5 * v(rho()) * umag2);
    ;
    v(p()) = tmp;
  }

  /// Inplace conversion of conservative to primitive variables
  template <typename PV> constexpr void cv_to_pv(PV&& v) const noexcept {
    cv_to_pv(std::forward<PV>(v), gamma_m1);
  }

  /// Speed of sound
  ///
  /// a = \sqrt{\gamma \frac{p}{\rho}}
  static constexpr num_t speed_of_sound(num_t gamma, num_t rho,
                                        num_t p) noexcept {
    return std::sqrt(gamma * p / rho);
  }

  /// Speed of sound
  constexpr num_t speed_of_sound(num_t rho, num_t p) const noexcept {
    return std::sqrt(gamma * p / rho);
  }

  /// Mach number
  static constexpr num_t mach_number(num_t velocity,
                                     num_t speed_of_sound) noexcept {
    return velocity / speed_of_sound;
  }

  /// Flux across a surface whose normal points in the d-th spatial dimension
  template <typename PV, typename F>
  static constexpr void flux_pv(PV&& pv, F&& f, suint_t d) noexcept {
    f(rho()) = pv(rho()) * pv(u(d));

    vels(f) = pv(rho()) * vels(pv) * pv(u(d));
    f(rho_u(d)) += pv(p());

    f(rho_E()) = pv(u(d)) * (energy_density(pv) + pv(p()));
  }

  /// Flux across a surface whose normal points in the d-th spatial dimension
  template <typename CV, typename F>
  static constexpr void flux_cv(CV&& cv, F&& f, num_t gamma_m1,
                                suint_t d) noexcept {
    f(rho()) = cv(rho_u(d));
    const num_t pres = pressure(gamma_m1, cv);
    const num_t ud   = velocity(cv, d);
    vels(f) = vels(cv) * ud;
    f(rho_u(d)) += pres;
    f(rho_E()) = ud * (cv(rho_E()) + pres);
  }

  ///@} // Physical relations

  /// \name Numerical method
  ///@{

  template <typename PV>
  static constexpr num_t compute_time_step_pv(PV&& v, euler<Nd, Flux> const& e,
                                              num_t length,
                                              num_t cfl) noexcept {
    num_t a = speed_of_sound(e.gamma, v(rho()), v(p()));

    /// Compute maximum wave speed: s_max = max(|u_d| + a)
    num_t s_max = (vels(v).array().abs() + a).maxCoeff();

    return cfl * length / s_max;
  }

  template <typename CV>
  static constexpr num_t compute_time_step_cv(CV&& cv, euler<Nd, Flux> const& e,
                                              num_t length,
                                              num_t cfl) noexcept {
    const num_t a
     = speed_of_sound(e.gamma, cv(rho()), pressure(e.gamma_m1, cv));

    /// Compute maximum wave speed: s_max = max(|u_d| + a)
    num_t s_max = (velocities(cv).array().abs() + a).maxCoeff();

    return cfl * length / s_max;
  }

  /// Local Lax-Friedrichs Flux
  ///
  /// Computes the \p d-th component of the Local-Lax_Friedrichs flux at
  /// the surface between the cells \p lIdx and \p rIdx. The distance between
  /// the cell centers is \p dx and the time-step is \p dt

  template <typename PV, typename F, typename Data,
            CONCEPT_REQUIRES_(Same<Flux, LxF>{})>
  static constexpr void numerical_flux_cv(PV&& cv_l, PV&& cv_r, F&& flux,
                                          suint_t d, Data&& c) noexcept {
    num_a<no_variables()> fL, fR;
    flux_cv(cv_l, fL, c.physics.gamma_m1, d);
    flux_cv(cv_r, fR, c.physics.gamma_m1, d);
    flux = (0.5 * (fL + fR + c.dx / c.dt * (cv_l - cv_r)));
    // flux = (0.5 * (fL + fR + c.dx / c.dt * (cv_l - cv_r)));
  }

  /// Advection Upstream Splitting Method (AUSM)
  ///
  /// Implemented after Liu & Steffen 1993
  /// TODO: cite paper
  template <typename PV, typename F, typename Data,
            CONCEPT_REQUIRES_(Same<Flux, AUSM>{})>
  constexpr void numerical_flux(PV&& pv_l, PV&& pv_r, F&& flux, suint_t d,
                                Data&& c) const noexcept {
    /// Helper functions for AUSM
    ///@{

    /// Signs required for the computation of the interface values
    constexpr auto pos = std::integral_constant<sint_t, +1>{};  // +
    constexpr auto neg = std::integral_constant<sint_t, -1>{};  // -

    /// Computes the interface Mach number \mathcal{M}^{+/-}
    ///
    /// Formula:
    ///                     1/2 * (mach  +/- |mach|), if |mach| > 1
    /// \mathcal{M}^{+/-} =
    ///                     +/- 1/4 * (mach +/- 1)^2, otherwise.
    ///
    /// TODO: simplified formula: 0.5 * ( ML + MR ) should be good enough
    const auto interface_mach = [](num_t mach, auto sign_t) -> num_t {
      /// TODO: manual constant folding
      constexpr num_t sign = decltype(sign_t){};
      const num_t mach_abs = std::abs(mach);
      return mach_abs > 1. ? 0.5 * (mach + sign * mach_abs)
                           : sign * 0.25 * std::pow(mach + sign, 2.);
    };

    /// Computes the interface pressure \mathcal{P}^{+-}
    ///
    /// Formula:
    ///                     1/2 * (mach  +/- |mach|) / mach, if |mach| > 1
    /// \mathcal{P}^{+/-} =
    ///                     1/4 * (mach +/- 1)^2 * (2 -/+ mach), otherwise.
    ///
    const auto interface_pressure = [](num_t mach, auto sign_t) -> num_t {
      /// TODO: manual constant folding
      constexpr num_t sign = decltype(sign_t){};
      const num_t mach_abs = std::abs(mach);
      return mach_abs > 1.
              ? 0.5 * (mach + sign * mach_abs) / mach
              : 0.25 * std::pow(mach + sign, 2.) * (2. - sign * mach);
    };

    /// Computes the d-th flux vector component divided by the
    /// d-th velocity (it
    /// is multiplied by the interface speed-of-sound in ausm)
    const auto theta
     = [](auto pv, num_t speed_of_sound, num_t gamma_m1, auto&& f) {
         auto cv = pv;
         pv_to_cv(cv, gamma_m1);
         f(rho()) = cv(rho());
         for (auto&& i : dimensions()) { f(rho_u(i)) = cv(rho_u(i)); }
         f(rho_E()) = cv(rho_E()) + pv(p());
         f *= speed_of_sound;
       };

    ///@} Helper functions for AUSM

    const num_t a_l = speed_of_sound(gamma, pv_l(rho()), pv_l(p()));
    const num_t a_r = speed_of_sound(gamma, pv_r(rho()), pv_r(p()));

    const num_t mach_l = mach_number(pv_l(u(d)), a_l);
    const num_t mach_r = mach_number(pv_r(u(d)), a_r);

    const num_t mach_i
     = interface_mach(mach_l, pos) + interface_mach(mach_r, neg);
    const num_t p_i = interface_pressure(mach_l, pos) * pv_l(p())
                      + interface_pressure(mach_r, neg) * pv_r(p());

    if (mach_i > 0.) {
      theta(pv_l, a_l, gamma_m1, flux);
    } else {
      theta(pv_r, a_r, gamma_m1, flux);
    }
    flux *= mach_i;

    flux(rho_u(d)) += p_i;

    flux *= c.area;
  }

  ///@}  // Numerical method

  /// \name Post-processing
  ///@{

  template <typename State, typename CellData>
  void load(State const& s, CellData&& cell_data) const noexcept {
    cell_data.load("density",
                   [&](cell_idx c, auto&&) { return s.variables(c)(rho()); });
    cell_data.load(
     "momentum_density",
     [&](cell_idx c, auto&& d) { return s.variables(c)(rho_u(d)); },
     dimension(), [&](auto&& d) { return dimension_name(d); });
    cell_data.load("energy_density",
                   [&](cell_idx c, auto&&) { return s.variables(c)(rho_E()); });
    // cell_data.load("pressure", [&](cell_idx c, auto&&) {
    //   return pressure(gamma_m1, s.variables(c));
    // });
    cell_data.load(
     "velocity",
     [&](cell_idx c, auto&& d) { return velocity(s.variables(c), d); },
     dimension(), [&](auto&& d) { return dimension_name(d); });
    // Gradient
    // Temperature
    // Mach number
    // Total energy
    // Specific internal energy
    // Enthalpy
    // Local speed of sound
  }

  ///@}  // Post-processing
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
