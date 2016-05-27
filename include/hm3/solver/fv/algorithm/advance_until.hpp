#pragma once
/// \file
///
/// Advance until a given time
#include <hm3/utility/math.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename State, typename BCs, typename NumFlux, typename TimeStep,
          typename Limiter, typename TimeStepFun>
uint_t advance_until(State& s, BCs& bcs, NumFlux&& nf, TimeStep&& ts,
                     Limiter&& lim, num_t time, num_t time_end,
                     uint_t time_step, num_t cfl,
                     TimeStepFun&& f = [](auto&&...) {}) {
  while (not math::approx(time, time_end)) {
    num_t dt = s.method.advance_once(s, bcs, nf, ts, lim, time_step, time,
                                     time_end, cfl);
    time += dt;
    time_step++;
    f(s, time_step);
  }
  return time_step;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
