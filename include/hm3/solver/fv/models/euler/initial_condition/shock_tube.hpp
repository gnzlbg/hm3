#pragma once
/// \file
///
///
#include <hm3/geometry/point.hpp>
#include <hm3/solver/fv/models/euler/indices.hpp>
#include <hm3/solver/fv/models/euler/pv.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

/// Initial conditions for the Euler equations
namespace ic {

/// Initial condition for shock tube problems
///
/// Can be customized along 2 directions only (x0, y1).
template <dim_t Nd>  //
struct shock_tube {
  using i       = indices<Nd>;
  using p       = pv_base<Nd>;
  using var_v   = num_a<i::nvars()>;
  using point_t = geometry::point<Nd>;

  num_t gamma = 1.4;  ///< Ratio of specific heats
  int_t dir   = 0;    ///< Direction: 0 => (L -> R), 1 => (R -> L)
  num_t angle = 0.;   ///< Angle for the sod shock in Nd > 1
  num_t x_0;          ///< Interface position normalized [0, 1]
  num_t alpha;        ///< Angle in radians.
  num_t pos;          ///< Interface position in the rotated system.
  var_v pv_l;
  var_v pv_r;

  /// \name Helpers
  ///@{
  CONCEPT_REQUIRES(Nd == 2)
  point_t rotate_x(point_t x) const noexcept {
    point_t x_rot;
    x_rot(0) = std::cos(alpha) * x(0) - std::sin(alpha) * x(1);
    x_rot(1) = std::sin(alpha) * x(0) + std::cos(alpha) * x(1);
    return x_rot;
  }

  CONCEPT_REQUIRES(Nd == 2)
  var_v rotate_u(num_t u_mag) const noexcept {
    var_v pv;
    p::u(pv, 0) = u_mag * std::cos(alpha);
    p::u(pv, 1) = u_mag * std::sin(alpha);
    return pv;
  }

  static constexpr num_t deg_to_rad(num_t angle_deg) noexcept {
    return angle_deg * (2. * math::pi<num_t>) / 360;
  }

  num_t pos_in_rot(num_t angle_rad) const noexcept {
    return math::approx(angle_rad, num_t{0.}) ? x_0 : x_0 * std::sqrt(2.);
  }

  ///@}  // Helpers

  constexpr shock_tube(num_t rho_l, num_t u_mag_l, num_t p_l,  //
                       num_t rho_r, num_t u_mag_r, num_t p_r,  //
                       num_t x_0_ = 0.5, num_t angle_ = 0.)
   : angle(std::move(angle_))
   , x_0(std::move(x_0_))
   , alpha(deg_to_rad(angle))
   , pos(pos_in_rot(alpha)) {
    pv_l = rotate_u(u_mag_l);
    pv_r = rotate_u(u_mag_r);

    p::rho(pv_l) = rho_l;
    p::p(pv_l)   = p_l;

    p::rho(pv_r) = rho_r;
    p::p(pv_r)   = p_r;
  }
  constexpr shock_tube(shock_tube const&) = default;
  constexpr shock_tube(shock_tube&&)      = default;

  constexpr shock_tube& operator=(shock_tube const&) = default;
  constexpr shock_tube& operator=(shock_tube&&) = default;

  /// Initial condition for Sod's shock-tube problem at point \p x
  var_v operator()(point_t x) const noexcept {
    point_t x_rot = rotate_x(x);
    var_v pv;
    if (x_rot(dir) < pos) {
      pv = pv_l;
    } else {
      pv = pv_r;
    }
    return p::to_cv(pv, gamma);
  };
};

/// Sod's test
///
///     rho  |   u   |  p
/// L:  1.0  | 0.0   | 1.0
/// R: 0.125 | 0.0   | 0.1
///
/// see Riemann Solvers and Numerical Methods for Fluid Dynamics 3rd
/// Edition p. 130
///
/// \note Mild test useful for assesing entropy satisfaction.
///
/// G. A. Sod, "A Survey of Several Finite Difference Methods for Systems of
/// Nonlinear Hyperbolic Conservation Laws", Journal of Computational Physics,
/// 27:1-31, 1978.
///
template <dim_t Nd> constexpr auto sod_shock_tube() {
  return shock_tube<Nd>(1.0, 0.0, 1.0,   // left state
                        0.125, 0.0, 0.1  // right state
                        );
}

/// Modified Sod's test
///
///     rho  |   u   |  p
/// L:  1.0  | 0.75  | 1.0
/// R: 0.125 | 0.0   | 0.1
///
/// see Riemann Solvers and Numerical Methods for Fluid Dynamics 3rd
/// Edition p. 225
///
/// \note this is a really easy problem. If something goes wrong here,
/// something fundamental is wrong.
///
template <dim_t Nd> auto modified_sod_shock_tube() {
  return shock_tube<Nd>(1.0, 0.75, 1.0,   // left state
                        0.125, 0.0, 0.1,  // right state
                        0.3);
}

/// 123 problem
///
///     rho  |   u   |  p
/// L:  1.0  | -2.0  | 0.4
/// R:  1.0  |  2.0  | 0.4
///
/// see Riemann Solvers and Numerical Methods for Fluid Dynamics 3rd
/// Edition p. 225
///
/// \note This is a tough problem that asses the performance for low-density
/// flows. The checks for negative density/pressure should fail before anything
/// goes wrong here.
///
template <dim_t Nd> auto one_two_three_shock_tube() {
  return shock_tube<Nd>(1.0, -2.0, 0.4,  // left state
                        1.0, 2.0, 0.4    // right state
                        );
}

}  // namespace ic

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
