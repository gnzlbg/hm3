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

template <uint_t Nd>  //
struct cv_base : dimensional<Nd>, equation_of_state {
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

  /// Velocity
  template <typename V>
  static constexpr decltype(auto) u(V&& v, suint_t d) noexcept {
    return v(i::rho_u(d)) / v(i::rho());
  }

  /// Velocities
  template <typename V> static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::dimension()>() / v(i::rho());
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
  template <typename V>
  static constexpr decltype(auto) p(V&& v, num_t gamma_m1) noexcept {
    return eos::pressure(gamma_m1, u_mag2(v), rho(v), rho_E(v));
  }
  /// Momentum density
  template <typename V> static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::dimension()>();
  }
  /// Momentum density
  template <typename V>
  static constexpr decltype(auto) rho_u(V&& v, suint_t d) noexcept {
    return v(i::rho_u(d));
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
  static constexpr var_v to_pv(V&& v, num_t gamma_m1) noexcept {
    var_v tmp;
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
    const num_t ud = rho_u(v, d) / rho(v);
    rho_u(f) = rho_u(v) * ud;
    rho_u(f, d) += p_;
    rho_E(f) = ud * (rho_E(v) + p_);
  }

  /// Flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V>
  static constexpr var_v flux(V&& v, suint_t d, num_t gamma_m1) noexcept {
    var_v f;
    flux_ip(std::forward<V>(v), f, d, gamma_m1);
    return f;
  }

  template <typename V>
  static constexpr num_t speed_of_sound(V&& v, num_t gamma,
                                        num_t gamma_m1) noexcept {
    return eos::speed_of_sound(gamma, rho(v), p(v, gamma_m1));
  }

  template <typename V>
  static constexpr num_t mach_number(V&& v, suint_t d, num_t gamma,
                                     num_t gamma_m1) noexcept {
    return eos::mach_number(u(v)(d).abs(), speed_of_sound(v, gamma, gamma_m1));
  }

  template <typename V>
  static constexpr num_t mach_number(V&& v, num_t gamma,
                                     num_t gamma_m1) noexcept {
    return eos::mach_number(u_mag(v), speed_of_sound(v, gamma, gamma_m1));
  }
};

template <uint_t Nd> struct cv : cv_base<Nd>, state<Nd> {
  using b = cv_base<Nd>;
  using state<Nd>::gamma;
  using state<Nd>::gamma_m1;
  using var_v = num_a<indices<Nd>::nvars()>;

  cv(state<Nd> s) : state<Nd>{std::move(s)} {}

  template <typename V> constexpr decltype(auto) p(V&& v) const noexcept {
    return b::p(std::forward<V>(v), gamma_m1);
  }

  template <typename V> constexpr auto to_pv(V&& v) const noexcept {
    return b::to_pv(std::forward<V>(v), gamma_m1);
  }

  template <typename V> constexpr void to_pv_ip(V&& v) const noexcept {
    b::to_pv(std::forward<V>(v), gamma_m1);
  }

  template <typename V, typename F>
  constexpr void flux_ip(V&& v, F&& f, suint_t d) const noexcept {
    b::flux_ip(std::forward<V>(v), std::forward<F>(f), d, gamma_m1);
  }
  template <typename V> constexpr auto flux(V&& v, suint_t d) const noexcept {
    return b::flux(std::forward<V>(v), d, gamma_m1);
  }

  template <typename V> constexpr num_t speed_of_sound(V&& v) const noexcept {
    return b::speed_of_sound(std::forward<V>(v), gamma, gamma_m1);
  }

  template <typename V>
  constexpr num_t mach_number(V&& v, suint_t d) const noexcept {
    return b::mach_number(std::forward<V>(v), d, gamma, gamma_m1);
  }

  template <typename V> constexpr num_t mach_number(V&& v) const noexcept {
    return b::mach_number(std::forward<V>(v), gamma, gamma_m1);
  }
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
