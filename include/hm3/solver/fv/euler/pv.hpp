#pragma once
/// \file
///
/// Interprets cell variables as primitive variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/equation_of_state.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/state.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <uint_t Nd>  //
struct pv_base : dimensional<Nd>, equation_of_state {
 private:
  using i     = indices<Nd>;
  using d     = dimensional<Nd>;
  using eos   = equation_of_state;
  using var_v = num_a<i::nvars()>;

 public:
  /// Density
  template <typename V> static constexpr decltype(auto) rho(V&& v) noexcept {
    return v(i::rho());
  }
  /// Velocities
  template <typename V> static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::dimension()>();
  }
  /// Velocity
  template <typename V>
  static constexpr decltype(auto) u(V&& v, suint_t d) noexcept {
    return v(i::u(d));
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V> static constexpr decltype(auto) u_mag2(V&& v) noexcept {
    return u(v).squaredNorm();
  }

  /// L2-Norm of the velocity ||u||_{L2}
  template <typename V> static constexpr decltype(auto) u_mag(V&& v) noexcept {
    return u(v).norm();
  }

  /// Pressure
  template <typename V> static constexpr decltype(auto) p(V&& v) noexcept {
    return v(i::p());
  }
  /// Momentum density
  template <typename V> static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::dimension()>() * v(i::rho());
  }
  /// Momentum density
  template <typename V>
  static constexpr decltype(auto) rho_u(V&& v, suint_t d) noexcept {
    return v(i::u(d)) * v(i::rho());
  }
  /// Energy density
  template <typename V>
  static constexpr decltype(auto) rho_E(V&& v, num_t gamma_m1) noexcept {
    return eos::energy_density(gamma_m1, u_mag2(v), rho(v), p(v));
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
  static constexpr var_v to_cv(V&& v, num_t gamma_m1) noexcept {
    var_v tmp(v);
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
  static constexpr var_v flux(V&& v, suint_t d, num_t gamma_m1) noexcept {
    var_v f;
    flux_ip(std::forward<V>(v), f, d, gamma_m1);
    return f;
  }

  template <typename V>
  static constexpr num_t speed_of_sound(V&& v, num_t gamma) noexcept {
    return eos::speed_of_sound(gamma, rho(v), p(v));
  }

  template <typename V>
  static constexpr num_t mach_number(V&& v, suint_t d, num_t gamma) noexcept {
    return eos::mach_number(u(v)(d).abs(), speed_of_sound(v, gamma));
  }

  template <typename V>
  static constexpr num_t mach_number(V&& v, num_t gamma) noexcept {
    return eos::mach_number(u_mag(v), speed_of_sound(v, gamma));
  }
};

template <uint_t Nd> struct pv : pv_base<Nd>, state<Nd> {
  using b = pv_base<Nd>;
  using state<Nd>::gamma;
  using state<Nd>::gamma_m1;
  using var_v = num_a<indices<Nd>::nvars()>;

  pv(state<Nd> s) : state<Nd>{std::move(s)} {}

  /// Energy density
  template <typename V> constexpr decltype(auto) rho_E(V&& v) const noexcept {
    return b::rho_E(std::forward<V>(v), gamma_m1);
  }

  /// Inplace conversion to conservative variables
  template <typename V> constexpr void to_cv_ip(V&& v) const noexcept {
    b::to_cv_ip(std::forward<V>(v), gamma_m1);
  }

  /// Conversion to conservative variables
  template <typename V> constexpr var_v to_cv(V&& v) const noexcept {
    return b::to_cv(std::forward<V>(v), gamma_m1);
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F>
  constexpr void flux_ip(V&& v, F&& f, suint_t d) const noexcept {
    b::flux_ip(std::forward<V>(v), std::forward<F>(f), d, gamma_m1);
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V> constexpr var_v flux(V&& v, suint_t d) const noexcept {
    return b::flux(std::forward<V>(v), d, gamma_m1);
  }

  template <typename V> constexpr num_t speed_of_sound(V&& v) const noexcept {
    return b::speed_of_sound(std::forward<V>(v), gamma);
  }

  template <typename V>
  constexpr num_t mach_number(V&& v, suint_t d) const noexcept {
    return b::mach_number(std::forward<V>(v), d, gamma);
  }

  template <typename V> constexpr num_t mach_number(V&& v) const noexcept {
    return b::mach_number(std::forward<V>(v), gamma);
  }
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
