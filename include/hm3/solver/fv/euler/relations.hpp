#pragma once
/// \file
///
///
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

struct equation_of_state {
  /// Pressure
  ///
  /// $ p = (\gamma - 1) \rho (E - ||u||_{L2}^2 / 2) $
  static constexpr num_t pressure(num_t gamma_m1, num_t u_mag2, num_t rho,
                                  num_t rho_E) noexcept {
    return gamma_m1 * (rho_E - 0.5 * rho * u_mag2);
  }

  /// Energy density
  ///
  /// $\rho E = p / (\gamma - 1) + \rho ||u||_{L2}^2 / 2 $
  static constexpr num_t energy_density(num_t gamma_m1, num_t u_mag2, num_t rho,
                                        num_t pressure) noexcept {
    return pressure / gamma_m1 + 0.5 * rho * u_mag2;
  }

  /// Speed of sound
  ///
  /// a = \sqrt{\gamma \frac{p}{\rho}}
  ///
  /// TODO: cannot be constexpr because of std::sqrt
  static num_t speed_of_sound(num_t gamma, num_t rho, num_t p) noexcept {
    return std::sqrt(gamma * p / rho);
  }

  /// Mach number
  ///
  /// $ u / a $
  static constexpr num_t mach_number(num_t velocity,
                                     num_t speed_of_sound) noexcept {
    return velocity / speed_of_sound;
  }
};

template <uint_t Nd>  //
struct primitive_variables {
 private:
  using i   = indices<Nd>;
  using d   = dimensional<Nd>;
  using eos = equation_of_state;

 public:
  /// Density
  template <typename V> static constexpr decltype(auto) rho(V&& v) noexcept {
    return v(i::rho());
  }
  /// Velocities
  template <typename V> static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::dimension()>();
  }
  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V> static constexpr decltype(auto) u_mag2(V&& v) noexcept {
    return u(v).squaredNorm();
  }
  /// Pressure
  template <typename V>
  static constexpr decltype(auto) pressure(V&& v) noexcept {
    return v(i::p());
  }
  /// Momentum density
  template <typename V> static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::dimension()>() * v(i::rho());
  }
  /// Energy density
  template <typename V>
  static constexpr decltype(auto) rho_E(V&& v, num_t gamma_m1) noexcept {
    return eos::energy_density(gamma_m1, u_mag2(v), rho(v), pressure(v));
  }

  /// Inplace conversion to conservative variables
  template <typename V>
  static constexpr void to_cv_ip(V&& v, num_t gamma_m1) noexcept {
    num_t umag2 = u_mag2(v);
    u(v) *= rho(v);
    p(v) = eos::energy_density(gamma_m1, std::move(umag2), rho(v), num_t{p(v)});
  }

  /// Conversion to conservative variables
  template <typename V>
  static constexpr num_a<i::nvars()> to_cv(V&& v, num_t gamma_m1) noexcept {
    num_a<i::nvars()> tmp(v);
    // rho(tmp) = rho(v);
    // u(tmp) = rho(v) * u(v);
    // p(tmp) = eos::energy_density(gamma_m1, u_mag2(v), rho(v), p(v));
    to_cv_ip(v, gamma_m1);
    return tmp;
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F>
  static constexpr void flux_ip(V&& v, F&& f, suint_t d,
                                num_t gamma_m1) noexcept {
    rho(f) = rho(v) * u(v)(d);
    u(f) = rho(v) * u(v) * u(v)(d);
    u(f)(d) += p(v);
    p(f) = u(v)(d) * (rho_E(v, gamma_m1) + p(v));
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V>
  static constexpr void flux(V&& v, suint_t d, num_t gamma_m1) noexcept {
    num_a<i::nvars()> f;
    flux_ip(v, f, d, gamma_m1);
    return f;
  }
};

template <uint_t Nd>  //
struct conservative_variables {
 private:
  using i   = indices<Nd>;
  using d   = dimensional<Nd>;
  using eos = equation_of_state;

 public:
  /// Density
  template <typename V> static constexpr decltype(auto) rho(V&& v) noexcept {
    return v(i::rho());
  }
  /// Velocities
  template <typename V> static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::dimension()>() / v(i::rho());
  }
  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V> static constexpr decltype(auto) u_mag2(V&& v) noexcept {
    return u(v).squaredNorm();
  }
  /// Pressure
  template <typename V>
  static constexpr decltype(auto) p(V&& v, num_t gamma_m1) noexcept {
    return eos::pressure(gamma_m1, u_mag2(v), rho(v), rho_E(v));
  }
  /// Momentum density
  template <typename V> static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::dimension()>();
  }
  /// Energy density
  template <typename V> static constexpr decltype(auto) rho_E(V&& v) noexcept {
    return v(i::rho_E());
  }

  /// Inplace conversion to primitive variables
  template <typename V>
  static constexpr void to_pv_ip(V&& v, num_t gamma_m1) noexcept {
    num_t umag2 = u_mag2_pv(v);
    rho_u(v) /= rho(v);
    rho_E(v) = eos::pressure(gamma_m1, umag2, rho(v), rhoE(v));
  }

  /// Conversion to primitive variables
  template <typename V>
  static constexpr num_a<i::nvars()> to_pv(V&& v, num_t gamma_m1) noexcept {
    num_a<i::nvars()> tmp;
    rho(tmp) = rho(v);
    rho_u(tmp) = rho_u(v) / rho(v);
    rho_E(tmp) = p(v, gamma_m1);
    return tmp;
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F>
  static constexpr void flux_ip(V&& v, F&& f, suint_t d,
                                num_t gamma_m1) noexcept {
    rho(f)         = rho_u(v)(d);
    const num_t p_ = p(v, gamma_m1);
    const num_t ud = rho_u(v)(d) / rho(v);
    rho_u(f) = rho_u(v) * ud;
    f(rho_u(d)) += p_;
    f(rho_E()) = ud * (v(rho_E()) + p_);
  }
};

template <uint_t Nd> struct euler : dimensional<Nd>, indices<Nd> {
  equation_of_state<Nd> eos;
  conservative_variables<Nd> cv;
  primitive_variables<Nd> pv;

  /// \name Physical constants
  ///@{
  num_t gamma;
  num_t gamma_m1;
  ///@}

  euler(num_t gamma_) : gamma{gamma_}, gamma_m1{gamma_ - 1.0} {
    HM3_ASSERT(gamma > 0., "negative gamma");
    HM3_ASSERT(gamma_m1 > 0., "negative gamma-1.0");
  }
};

template <uint_t Nd> struct numerical_pv : indices<Nd>, dimensional<Nd> {
  using indices<Nd>::rho;
  using indices<Nd>::p;
  using indices<Nd>::velocities;
  using eos = equation_of_state;

  template <typename V>
  static constexpr num_t time_step(V&& v, euler<Nd> const& e, num_t length,
                                   num_t cfl) noexcept {
    num_t a = equation_of_state::speed_of_sound(e.gamma, v(rho()), v(p()));

    /// Compute maximum wave speed: s_max = max(|u_d| + a)
    num_t s_max = (velocities(v).array().abs() + a).maxCoeff();

    return cfl * length / s_max;
  }

  /// Local Lax-Friedrichs Flux
  ///
  /// Computes the \p d-th component of the Local-Lax_Friedrichs flux at
  /// the surface between the cells \p lIdx and \p rIdx. The distance between
  /// the cell centers is \p dx and the time-step is \p dt
  template <typename V, typename F, typename Data>
  static constexpr void local_lax_friedrichs(PV&& pv_l, V&& pv_r, F&& flux,
                                             suint_t d, Data&& c) noexcept {
    num_a<no_variables()> fL, fR;
    // flux_cv(cv_l, fL, c.physics.gamma_m1, d);
    // flux_cv(cv_r, fR, c.physics.gamma_m1, d);
    // flux = (0.5 * (fL + fR + c.dx / c.dt * (cv_l - cv_r)));
    // flux = (0.5 * (fL + fR + c.dx / c.dt * (cv_l - cv_r)));
  }
};

template <uint_t Nd> struct numerical_cv : indices<Nd>, dimensional<Nd> {
  using indices<Nd>::rho;
  using indices<Nd>::p;
  using indices<Nd>::velocities;
  using eos   = equation_of_state;
  using cv    = conservative_variables<Nd>;
  using var_v = num_a<indices<Nd>::nvars()>;

  template <typename V>
  static constexpr num_t time_step(V&& v, euler<Nd> const& e, num_t length,
                                   num_t cfl) noexcept {
    const num_t a = speed_of_sound(e.gamma, v(rho()), pressure(e.gamma_m1, v));

    /// Compute maximum wave speed: s_max = max(|u_d| + a)
    num_t s_max = (velocities(v).array().abs() + a).maxCoeff();

    return cfl * length / s_max;
  }

  /// Local Lax-Friedrichs Flux
  ///
  /// Computes the \p d-th component of the Local-Lax_Friedrichs flux at
  /// the surface between the cells \p lIdx and \p rIdx. The distance between
  /// the cell centers is \p dx and the time-step is \p dt
  template <typename V, typename F, typename Data>
  static constexpr var_v local_lax_friedrichs(V&& v_l, V&& v_r, suint_t d,
                                              Data&& c) noexcept {
    var_v fL = cv::flux_(v_l, c.physics.gamma_m1, d);
    var_v fR = cv::flux_(v_r, c.physics.gamma_m1, d);
    return 0.5 * (fL + fR + c.dx / c.dt * (cv_l - cv_r));
  }
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
