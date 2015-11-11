#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/equation_of_state.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/state.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

/// Local Lax-Friedrichs Flux
///
/// Computes the \p d-th component of the Local-Lax_Friedrichs flux at an
/// interface with left \p v_l and right \p v_r states.
template <typename V, typename VT, typename State>
static constexpr auto local_lax_friedrichs(VT&& vt, V&& v_l, V&& v_r, suint_t d,
                                           State&& s) noexcept {
  auto fL = vt.flux(v_l, d);
  auto fR = vt.flux(v_r, d);
  return (0.5 * (fL + fR + s.dx / s.dt * (v_l - v_r)));
}

/// Advection Upstream Splitting Method (AUSM)
///
/// Implemented after Liu & Steffen 1993
/// TODO: cite paper
template <typename V, typename VT, typename State,
          typename var_v = num_a<VT::nvars()>>
constexpr var_v ausm(VT&& vt, V&& v_l, V&& v_r, suint_t d, State&& s) noexcept {
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

  /// Computes the d-th flux vector component divided by the d-th velocity (it
  /// is multiplied by the interface speed-of-sound in ausm)
  const auto theta = [](auto&& vt, auto&& v, num_t speed_of_sound) {
    var_v f;
    vt.rho(f) = vt.rho(v);
    vt.rho_u(f) = vt.rho_u(v);
    vt.rho_E(f) = vt.rho_E(v) + vt.p(v);
    f *= speed_of_sound;
    return f;
  };

  ///@} Helper functions for AUSM

  const num_t a_l = vt.speed_of_sound(v_l);
  const num_t a_r = vt.speed_of_sound(v_r);

  const num_t mach_l = vt.mach_number(vt.u(v_l, d), a_l);
  const num_t mach_r = vt.mach_number(vt.u(v_r, d), a_r);

  const num_t mach_i
   = interface_mach(mach_l, pos) + interface_mach(mach_r, neg);
  const num_t p_i = interface_pressure(mach_l, pos) * vt.p(v_l)
                    + interface_pressure(mach_r, neg) * vt.p(v_r);

  var_v f = mach_i > 0. ? theta(v_l, a_l) : theta(v_r, a_r);

  f *= mach_i;

  vt.rho_u(f, d) += p_i;

  return f;
}

}  // namespace euler

}  // namespace fv
}  // namespace solver
}  // namespace hm3
