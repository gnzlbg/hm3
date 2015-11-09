#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/cv.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/pv.hpp>
#include <hm3/solver/fv/euler/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <uint_t Nd> struct physics : state<Nd>, dimensional<Nd>, indices<Nd> {
  physics(num_t gamma) : state<Nd>{std::move(gamma)} {}

  using cv_t = cv<Nd>;
  using pv_t = pv<Nd>;

  cv_t cv() const noexcept { return cv_t{*this}; }
  pv_t pv() const noexcept { return pv_t{*this}; }

  template <typename S, typename V, typename CellData>
  static void load(V&& v, S const& s, CellData&& cell_data) noexcept {
    cell_data.load("density",
                   [&](auto c, auto&&) { return v.rho(s.variables(c)); });
    cell_data.load("momentum_density",
                   [&](auto c, auto&& d) { return v.rho_u(s.variables(c), d); },
                   v.dimension(),
                   [&](auto&& d) { return v.dimension_name(d); });
    cell_data.load("energy_density",
                   [&](auto c, auto&&) { return v.rho_E(s.variables(c)); });
    // cell_data.load("pressure", [&](auto c, auto&&) {
    //   return pressure(gamma_m1, s.variables(c));
    // });
    cell_data.load(
     "velocity", [&](auto c, auto&& d) { return v.u(s.variables(c), d); },
     v.dimension(), [&](auto&& d) { return v.dimension_name(d); });
    // Gradient
    // Temperature
    // Mach number
    // Total energy
    // Specific internal energy
    // Enthalpy
    // Local speed of sound
  }
};

}  // namespace euler

}  // namespace fv
}  // namespace solver
}  // namespace hm3
