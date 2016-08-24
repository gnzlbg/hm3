#pragma once
/// \file
///
/// Analytical solution for the temperature on a 1D slab
#include <hm3/geometry/primitive/point.hpp>
#include <hm3/solver/fv/models/heat/indices.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace heat {

/// Analytical solutions for the heat equation
namespace as {

/// Analytical solution on a 1D slab
template <dim_t Nd>
struct slab {
  using i       = indices<Nd>;
  using point_t = geometry::point<Nd>;
  using var_v   = num_a<i::nvars()>;

  /// Constant heat diffusivity of the material
  num_t thermal_diffusivity = 1.0;
  num_t domain_length       = 1.0;
  num_t temperature_left    = 0.0;
  num_t temperature_right   = 0.0;
  suint_t no_iterations     = 100;
  num_t int_res;
  vector<num_t> cns;

  template <typename F>
  slab(F&& f, suint_t no_iters = 1000)
   : no_iterations(no_iters), cns(no_iters + 1) {
    initialize(std::forward<F>(f));
  }

  template <typename F>
  void initialize(F&& initial_condition) {
    for (suint_t n = 1; n <= no_iterations; ++n) {
      auto f = [&](num_t x) {
        HM3_ASSERT(x >= 0. and x <= 1, "");
        return initial_condition(x)
               * std::sin(n * math::pi * x / domain_length);
      };
      cns[n] = math::integrate(f, 0., domain_length);
    }
  }

  num_t operator()(point_t x_v, num_t time) const noexcept {
    num_t x = x_v(0);

    num_t f0 = -thermal_diffusivity * std::pow(math::pi, 2.) * time
               / std::pow(domain_length, 2.);

    num_t a = 0;
    for (suint_t n = 1; n <= no_iterations; ++n) {
      a += (temperature_right * std::cos(n * math::pi) - temperature_left) / n
           * std::sin(n * math::pi * x / domain_length) * std::exp(f0 * n * n);
    }
    a *= 2. / math::pi;

    num_t b = 0;
    for (suint_t n = 1; n <= no_iterations; ++n) {
      b += std::sin(n * math::pi * x / domain_length) * std::exp(f0 * n * n)
           * cns[n];
    }
    b *= 2. / domain_length;

    num_t temperature
     = temperature_left
       + (x / domain_length) * (temperature_right - temperature_left) + a + b;

    return temperature;
  }
};

}  // namespace as
}  // namespace heat
}  // namespace fv
}  // namespace solver
}  // namespace hm3
