#pragma once
/// \file
///
/// Zero the slopes (disables the limiter)
#include <hm3/solver/fv/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace limiter {

struct zero_fn {
  template <typename Grad,
            typename vars = num_a<std::decay_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return vars::Zero();
  }
};

namespace {
constexpr auto&& zero = static_const<zero_fn>::value;
}  // namespace

}  // namespace limiter
}  // namespace fv
}  // namespace solver
}  // namespace hm3
