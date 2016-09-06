#pragma once
/// \file
///
/// Integrates a 1D function up-to a specific tolerance
#include <hm3/math/integral/simpson_1d.hpp>

namespace hm3::math {

namespace math_detail {
/// Integrates a 1D function up-to a specific tolerance
struct integrate_1d_fn {
  template <typename F>
  constexpr num_t operator()(F&& f, num_t from, num_t to, num_t tol = 1e-12,
                             suint_t max_subdivisions
                             = std::numeric_limits<suint_t>::max() / 100) const
   noexcept {
    HM3_ASSERT(from < to,
               "domain-error: [from, to) = [{}, {}) (note: {} !< {})", from, to,
               from, to);
    suint_t subdiv = 4;
    num_t result   = integrate_simpson(f, from, to, subdiv);
    while (true) {
      subdiv *= 2;
      num_t next_result = integrate_simpson(f, from, to, subdiv);
      if (std::abs(result - next_result) < tol) { return next_result; }
      result = next_result;
      if (subdiv > max_subdivisions) {
        // note: if you need more subdivisions you should be calling
        HM3_FATAL_ERROR(
         "maximum number of subdivisions exceeded while integrating");
      }
    }
  }
};

}  // namespace math_detail

namespace {
static constexpr auto const& integrate_1d
 = static_const<math_detail::integrate_1d_fn>::value;
}  // namespace

}  // namespace hm3::math
