#pragma once
/// \file
///
/// Trapezoidal rule for 1D functions
#include <hm3/math/approx.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::math {

namespace math_detail {
/// Integrates a 1D function using the trapezoidal rule
///
/// \pre subdivisions > 2
struct integrate_trapezoidal_fn {
  template <typename F>
  constexpr num_t operator()(F&& f, num_t from, num_t to,
                             suint_t subdivisions) const noexcept {
    HM3_ASSERT(from < to,
               "domain-error: [from, to) = [{}, {}) (note: {} !< {})", from, to,
               from, to);
    HM3_ASSERT(subdivisions > 2, "subdivisions ({}) must be greater than 2",
               subdivisions);

    num_t dx = (to - from) / subdivisions;
    HM3_ASSERT(math::approx(from + dx * subdivisions, to), "");

    num_t value = f(from) / 2.;
    for (suint_t i = 1; i < subdivisions; ++i) {
      num_t x = from + i * dx;
      value += f(x);
    }

    value += f(to) / 2.;
    value *= dx;

    return value;
  }
};

}  // namespace math_detail

namespace {
static constexpr auto const& integrate_trapezoidal
 = static_const<math_detail::integrate_trapezoidal_fn>::value;
}  // namespace

}  // namespace hm3::math
