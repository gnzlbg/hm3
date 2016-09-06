#pragma once
/// \file
///
/// Simpson Rule for 1D integrals
#include <hm3/types.hpp>
#include <hm3/utility/config/assert.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::math {

namespace math_detail {
/// Integrates a 1D function using the simpson rule
///
/// \pre subdivisions > 3 and subdivisions must be an even number
struct integrate_simpson_fn {
  template <typename F>
  constexpr num_t operator()(F&& f, num_t from, num_t to,
                             suint_t subdivisions) const noexcept {
    HM3_ASSERT(from < to,
               "domain-error: [from, to) = [{}, {}) (note: {} !< {})", from, to,
               from, to);

    num_t dx = (to - from) / subdivisions;
    HM3_ASSERT(math::approx(from + dx * subdivisions, to), "");
    HM3_ASSERT(subdivisions > 3, "subdivisions ({}) must be greater than 3",
               subdivisions);
    HM3_ASSERT(subdivisions % 2 == 0, "subdivisions ({}) must be even",
               subdivisions);

    num_t value = f(from);
    for (suint_t i = 1; i < subdivisions; i += 2) {
      value += 4. * f(from + i * dx);
      value += 2. * f(from + (i + 1) * dx);
    }

    value += f(to);
    value *= dx / 3.;

    return value;
  }
};

}  // namespace math_detail

namespace {
static constexpr auto const& integrate_simpson
 = static_const<math_detail::integrate_simpson_fn>::value;
}  // namespace

}  // namespace hm3::math
