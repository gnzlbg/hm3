#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimension.hpp>
#include <hm3/solver/fv/models/euler/equation_of_state.hpp>
#include <hm3/solver/fv/models/euler/indices.hpp>
#include <hm3/solver/fv/models/euler/state.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

namespace flux {

/// Helper functions for AUSM flux computation
namespace ausm_detail {

/// Computes the interface Mach number \mathcal{M}^{+/-}
///
/// Formula:
///                     1/2 * (mach  +/- |mach|), if |mach| > 1
/// \mathcal{M}^{+/-} =
///                     +/- 1/4 * (mach +/- 1)^2, otherwise.
///
/// TODO: simplified formula: 0.5 * ( ML + MR ) should be good enough
template <typename Sign>
constexpr num_t interface_mach(num_t mach, Sign) noexcept {
  /// TODO: manual constant folding
  constexpr num_t sign = Sign{};
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
template <typename Sign>
constexpr num_t interface_pressure(num_t mach, Sign) noexcept {
  /// TODO: manual constant folding
  constexpr num_t sign = Sign{};
  const num_t mach_abs = std::abs(mach);
  return mach_abs > 1. ? 0.5 * (mach + sign * mach_abs) / mach
                       : 0.25 * std::pow(mach + sign, 2.) * (2. - sign * mach);
};

/// Computes the d-th flux vector component divided by the d-th velocity (it
/// is multiplied by the interface speed-of-sound in ausm)
template <typename VT, typename V,
          typename var_v = num_a<uncvref_t<VT>::nvars()>>
constexpr var_v theta(VT&& vt, V&& v, num_t speed_of_sound) noexcept {
  var_v f;
  vt.rho(f)   = vt.rho(v);
  vt.rho_u(f) = vt.rho_u(v);
  vt.rho_E(f) = vt.rho_E(v) + vt.p(v);
  f *= speed_of_sound;
  return f;
};

}  // namespace ausm_detail

struct ausm_fn {
  /// Advection Upstream Splitting Method (AUSM)
  ///
  /// Liou, M.-S. and Steffen, C., "A New Flux Splitting Scheme", J. Comput.
  /// Phys., Vol. 107, 23-39, 1993.
  template <typename V, typename VT, typename State,
            typename var_v = num_a<uncvref_t<VT>::nvars()>>
  constexpr var_v operator()(VT&& vt, V&& v_l, V&& v_r, dim_t d,
                             State&& /*s*/) const noexcept {
    using ausm_detail::interface_mach;
    using ausm_detail::interface_pressure;
    using ausm_detail::theta;

    /// Signs required for the computation of the interface values
    constexpr auto p = std::integral_constant<sint_t, +1>{};  // +
    constexpr auto n = std::integral_constant<sint_t, -1>{};  // -

    const num_t a_l = vt.speed_of_sound(v_l);
    const num_t a_r = vt.speed_of_sound(v_r);
    const num_t u_l = vt.u(v_l, d);
    const num_t u_r = vt.u(v_r, d);

    const num_t mach_l = vt.mach_number(u_l, a_l);
    const num_t mach_r = vt.mach_number(u_r, a_r);

    const num_t mach_i = interface_mach(mach_l, p) + interface_mach(mach_r, n);
    const num_t p_i    = interface_pressure(mach_l, p) * vt.p(v_l)
                      + interface_pressure(mach_r, n) * vt.p(v_r);

    var_v f = mach_i > 0. ? theta(vt, v_l, a_l) : theta(vt, v_r, a_r);
    f *= mach_i;
    vt.rho_u(f, d) += p_i;

    return f;
  }
};

namespace {
constexpr auto&& ausm = static_const<ausm_fn>::value;
}  // namespace

}  // namespace flux
}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
