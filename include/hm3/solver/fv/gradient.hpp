#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct gradient_fn {
  template <typename LeftVars, typename RightVars>
  constexpr decltype(auto) operator()(LeftVars&& vL, RightVars&& vR,
                                      num_t const& distance) const noexcept {
    return (vR - vL) / distance;
  }
};

namespace {
constexpr auto&& gradient = static_const<gradient_fn>::value;
}  // namespace

}  // namespace fv
}  // namespace solver
}  // namespace hm3
