#ifdef FIXED
#pragma once
/// \file
///
/// Zero the slopes (disables the limiter)
#include <hm3/solver/fv/state.hpp>

namespace hm3::solver::fv::limiter {

struct zero_fn {
  template <typename Grad,
            typename vars = num_a<uncvref_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return vars::Zero();
  }
};

namespace {
constexpr auto const& zero = static_const<zero_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv::limiter
#endif
