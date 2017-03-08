#pragma once
/// \file
///
/// Relative position of a primitive w.r.t. a signed-distance field.
#include <hm3/geometry/algorithm/approx/number.hpp>
#include <hm3/geometry/tolerance.hpp>
#include <hm3/math/core.hpp>

namespace hm3::geometry::sd {

namespace sign_detail {

struct sign_fn {
  /// Approximate sign of the signed-distance \p value.
  constexpr math::signum_t operator()(num_t value, num_t abs_tol,
                                      num_t rel_tol) const noexcept {
    if (approx_number(value, 0., abs_tol, rel_tol)) {
      return math::signum_t::zero();
    }
    return math::signum(value);
  }
};

}  // namespace sign_detail

namespace {
static constexpr auto const& sign
 = static_const<with_default_tolerance<sign_detail::sign_fn>>::value;
}  // namespace

}  // namespace hm3::geometry::sd
