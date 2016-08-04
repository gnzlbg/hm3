#pragma once
/// \file
///
/// Analytical solution for an isentropic vortex
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/solver/fv/models/euler/indices.hpp>
#include <hm3/solver/fv/models/euler/pv.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

/// Analytical solutions for the Euler equations
namespace as {

template <dim_t Nd>  //
struct isentropic_vortex {
  using i       = indices<Nd>;
  using p       = pv_base<Nd>;
  using point_t = geometry::point<Nd>;
  using var_v   = num_a<i::nvars()>;

  num_t beta  = 5.;
  num_t gamma = 1.4;                     ///< Ratio of specific heats.
  point_t x_0 = point_t::constant(0.5);  ///< Initial vortex position.
  point_t u   = point_t::constant(1.0);  ///< Constant parallel flow velocity.

  /// Analytical solution for an isentropic vortex in a parallel flow
  ///
  /// \param x [in] Position at which to compute the analytical solution.
  /// \param t [in] Time at which to compute the analytical solution.
  ///
  /// \returns Conservative variables.
  ///
  /// \todo Where is this from? (Nodal Disc. Gal. Methods book?)
  CONCEPT_REQUIRES(Nd == 2)
  var_v operator()(point_t x, num_t t) const noexcept {
    var_v pv;

    constexpr auto pi = math::pi;

    point_t x_t = x() - u() * t;
    num_t r     = (x_t() - x_0()).norm();

    num_t f0 = std::exp(1. - std::pow(r, 2.));
    num_t f1 = std::exp(2. * f0);

    /// \todo u x (beta * f * (x_t - x_0)) / (2pi) ?
    p::u(pv, 0) = u(0) - beta * f0 * (x_t(1) - x_0(1)) / (2 * pi);
    p::u(pv, 1) = u(1) + beta * f0 * (x_t(0) - x_0(0)) / (2 * pi);

    p::rho(pv) = std::pow((1.
                           - ((gamma - 1.) * std::pow(beta, 2.) * f1)
                              / (16. * gamma * std::pow(pi, 2.))),
                          1. / (gamma - 1.));
    p::p(pv) = std::pow(p::rho(pv), gamma);
    return p::to_cv(pv, gamma);
  }
};

}  // namespace as

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
