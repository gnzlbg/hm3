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
struct pv_base : dimensional<Nd>, equation_of_state, indices<Nd> {
  using equation_of_state::mach_number;
  using equation_of_state::energy_density;
  using equation_of_state::speed_of_sound;

 private:
  using i = indices<Nd>;
  using d = dimensional<Nd>;

 public:
  using vars = num_a<i::nvars()>;

  /// Density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho(V&& v) noexcept {
    return v(i::rho());
  }
  /// Velocities
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::dimension()>();
  }
  /// Velocity
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u(V&& v, suint_t d) noexcept {
    return v(i::u(d));
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u_mag2(V&& v) noexcept {
    return u(v).squaredNorm();
  }

  /// L2-Norm of the velocity ||u||_{L2}
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u_mag(V&& v) noexcept {
    return u(v).norm();
  }

  /// Pressure
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) p(V&& v) noexcept {
    return v(i::p());
  }
  /// Momentum density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::dimension()>() * v(i::rho());
  }
  /// Momentum density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_u(V&& v, suint_t d) noexcept {
    return v(i::u(d)) * v(i::rho());
  }
  /// Energy density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t rho_E(V&& v, num_t gamma_m1) noexcept {
    return energy_density(gamma_m1, u_mag2(v), rho(v), p(v));
  }

  /// Inplace conversion to conservative variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr void to_cv_ip(V&& v, num_t gamma_m1) noexcept {
    num_t umag2 = u_mag2(v);
    u(v) *= rho(v);
    p(v) = energy_density(gamma_m1, std::move(umag2), rho(v), num_t{p(v)});
  }

  /// Conversion to conservative variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr vars to_cv(V&& v, num_t gamma_m1) noexcept {
    vars tmp;
    rho(tmp) = rho(v);
    u(tmp)   = rho(v) * u(v);
    p(tmp)   = energy_density(gamma_m1, u_mag2(v), rho(v), p(v));
    return tmp;
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F,
            CONCEPT_REQUIRES_(!rvref<V&&> and !rvref<F&&>)>
  static constexpr void flux_ip(V&& v, F&& f, suint_t d,
                                num_t gamma_m1) noexcept {
    const num_t u_d = u(v)(d);
    rho(f)          = rho(v) * u_d;
    u(f)            = rho(v) * u(v) * u_d;
    u(f)(d) += p(v);
    p(f) = u_d * (rho_E(v, gamma_m1) + p(v));
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr vars flux(V&& v, suint_t d, num_t gamma_m1) noexcept {
    vars f;
    flux_ip(v, f, d, gamma_m1);
    return f;
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t speed_of_sound(V&& v, num_t gamma) noexcept {
    return speed_of_sound(gamma, rho(v), p(v));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t mach_number(V&& v, suint_t d, num_t gamma) noexcept {
    return mach_number(u(v)(d).abs(), speed_of_sound(v, gamma));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t mach_number(V&& v, num_t gamma) noexcept {
    return mach_number(u_mag(v), speed_of_sound(v, gamma));
  }
};

template <uint_t Nd> struct pv : pv_base<Nd>, state {
  using b = pv_base<Nd>;
  using state::gamma;
  using state::gamma_m1;
  using pv_base<Nd>::mach_number;
  using typename pv_base<Nd>::vars;

  pv(state s) : state{std::move(s)} {}

  /// Energy density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr auto rho_E(V&& v) const noexcept {
    return b::rho_E(v, gamma_m1);
  }

  /// Inplace conversion to conservative variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr void to_cv_ip(V&& v) const noexcept {
    b::to_cv_ip(v, gamma_m1);
  }

  /// Conversion to conservative variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr vars to_cv(V&& v) const noexcept {
    return b::to_cv(v, gamma_m1);
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F,
            CONCEPT_REQUIRES_(!rvref<V&&> and !rvref<F&&>)>
  constexpr void flux_ip(V&& v, F&& f, suint_t d) const noexcept {
    b::flux_ip(v, std::forward<F>(f), d, gamma_m1);
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr vars flux(V&& v, suint_t d) const noexcept {
    return b::flux(v, d, gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t speed_of_sound(V&& v) const noexcept {
    return b::speed_of_sound(v, gamma);
  }

  template <typename V,
            CONCEPT_REQUIRES_(
             !rvref<V&&> and !std::is_floating_point<std::decay_t<V>>{})>
  constexpr num_t mach_number(V&& v, suint_t d) const noexcept {
    return b::mach_number(v, d, gamma);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t mach_number(V&& v) const noexcept {
    return b::mach_number(v, gamma);
  }
};

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
