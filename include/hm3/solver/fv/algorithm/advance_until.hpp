#pragma once
/// \file
///
/// Advance until a given time
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct advance_until_fn {
  template <typename State, typename BCs, typename TimeStep, typename Limiter,
            typename TimeStepFun>
  constexpr uint_t operator()(State& s, BCs& bcs, TimeStep&& ts, Limiter&& lim,
                              num_t time, num_t time_end, uint_t time_step,
                              num_t cfl, TimeStepFun&& f
                                         = [](auto&&...) {}) const noexcept {
    while (not math::approx(time, time_end)) {
      num_t dt
       = s.method.advance_once(s, bcs, ts, lim, time_step, time, time_end, cfl);
      time += dt;
      time_step++;
      f(s, time, time_step);
    }
    return time_step;
  }
};

namespace {
static constexpr auto&& advance_until = static_const<advance_until_fn>::value;
}  // namespace

}  // namespace fv
}  // namespace solver
}  // namespace hm3
