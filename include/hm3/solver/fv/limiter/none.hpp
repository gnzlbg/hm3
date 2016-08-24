#pragma once
/// \file
///
/// No slope limiter (slopes are always multiplied by "ones")
#include <hm3/solver/fv/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace limiter {

struct none_fn {
  template <typename Grad,
            typename Vars = num_a<uncvref_t<Grad>::SizeAtCompileTime>>
  Vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return Vars::Ones();
  }
};

namespace {
constexpr auto&& none = static_const<none_fn>::value;
}  // namespace

}  // namespace limiter
}  // namespace fv
}  // namespace solver
}  // namespace hm3
