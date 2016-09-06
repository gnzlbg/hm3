#pragma once
/// \file
///
/// Gradient
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::math {

namespace math_detail {

struct gradient_fn {
  template <typename LeftVars, typename RightVars>
  constexpr auto operator()(LeftVars&& v_l, RightVars&& v_r,
                            num_t const& distance) const noexcept {
    return (v_r - v_l) / distance;
  }
};

}  // namespace math_detail

namespace {
constexpr auto const& gradient = static_const<math_detail::gradient_fn>::value;
}  // namespace

}  // namespace hm3::math
