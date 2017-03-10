#pragma once
/// \file
///
/// Gaussian distribution
#include <cmath>
#include <hm3/math/constants.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::math {

namespace math_detail {

/// Computes an n-dimensional elliptical gaussian function
struct gaussian_fn {
  /// Computes an n-dimensional elliptical gaussian function
  ///
  /// Sigma is the spread
  /// mu is the center
  ///
  /// f(x) = \frac {1}{\sqrt{2 \pi}^Ad \prod \sigma_i} \exp{- \left ( \sum
  /// \frac{(x_i - \mu_i)^2}{2 \sigma_i^2} \right )}
  template <dim_t Ad>
  constexpr num_t operator()(num_a<Ad> x, num_a<Ad> mu, num_a<Ad> sigma) const
   noexcept {
    // compute the exponent:
    num_t exponent = 0.;
    for (dim_t d = 0; d < Ad; ++d) {
      exponent += std::pow(x(d) - mu(d), 2.) / (2. * std::pow(sigma(d), 2.));
    }
    exponent *= -1.;

    // compute the front factor:
    num_t factor = 1. / std::pow(std::sqrt(2. * pi), Ad);
    for (dim_t d = 0; d < Ad; ++d) { factor *= 1. / sigma(d); }

    return factor * std::exp(exponent);
  }

  /// Computes an n-dimensional gaussian function with constant sigma:
  ///
  /// f(x) = \frac {1}{(\sqrt{2 \pi} \sigma)^Ad} \exp{- \left ( \sum
  /// (x_i - \mu_i)^2 \right ) / (2 \sigma^2)}
  template <dim_t Ad>
  constexpr num_t operator()(num_a<Ad> x, num_a<Ad> mu, num_t sigma) const
   noexcept {
    return gaussian(x, mu, num_a<Ad>::constant(sigma));
  }
};

}  // namespace math_detail

namespace {
constexpr auto const& gaussian = static_const<math_detail::gaussian_fn>::value;
}  // namespace

}  // namespace hm3::math
