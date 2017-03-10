#ifdef FIXED
#pragma once
/// \file
///
/// No slope limiter (slopes are always multiplied by "ones")
#include <hm3/solver/fv/state.hpp>

namespace hm3::solver::fv::limiter {

struct none_fn {
  template <typename Grad,
            typename Vars = num_a<uncvref_t<Grad>::SizeAtCompileTime>>
  Vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return Vars::Ones();
  }
};

namespace {
constexpr auto const& none = static_const<none_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv::limiter
#endif
