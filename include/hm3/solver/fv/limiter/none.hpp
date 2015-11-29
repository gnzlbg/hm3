#pragma once
/// \file
///
/// No slope limiter (slopes are always multiplied by "ones")
#include <hm3/solver/fv/state.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace limiter {

struct none_fn {
  template <typename Grad,
            typename vars = num_a<std::decay_t<Grad>::SizeAtCompileTime>>
  vars operator()(Grad&& /*g_l*/, Grad&& /*g_r*/) const noexcept {
    return vars::Ones();
  }
};

namespace {
constexpr auto&& none = static_const<none_fn>::value;
}  // namespace

}  // namespace limiter
}  // namespace fv
}  // namespace solver
}  // namespace hm3
