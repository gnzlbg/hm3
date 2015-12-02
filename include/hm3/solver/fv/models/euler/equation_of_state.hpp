#pragma once
/// \file
///
///
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

struct equation_of_state {
  /// Pressure
  ///
  /// $ p = (\gamma - 1) \rho (E - ||u||_{L2}^2 / 2) $
  static constexpr num_t pressure(num_t gamma_m1, num_t rho_u_mag2, num_t rho,
                                  num_t rho_E) noexcept {
    return gamma_m1 * (rho_E - 0.5 * rho_u_mag2 / rho);
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

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
