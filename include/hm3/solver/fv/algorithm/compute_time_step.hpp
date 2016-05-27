#pragma once
/// \file
///
/// Compute time step
#include <hm3/types.hpp>
#include <limits>

namespace hm3 {
namespace solver {
namespace fv {

template <typename State, typename TimeStepF, typename V>
num_t compute_time_step(State const& s, num_t cfl, TimeStepF&& tf, V&& v) {
  num_t dt = std::numeric_limits<num_t>::max();
  for (auto&& t : s.tiles()) {
    auto&& lhs = s.time_integration.lhs(t);
    auto dx    = t.geometry().cell_length();
    t.cells().for_each_internal(
     [&](auto c) { dt = std::min(dt, tf(lhs(c), v, dx, cfl)); });
  }
  return dt;
}

template <typename State, typename TimeStepF, typename V>
num_t compute_time_step(State const& s, num_t cfl, TimeStepF&& t, V&& v,
                        num_t time, num_t time_end) {
  num_t dt = compute_time_step(s, cfl, t, v);
  if (time + dt > time_end) { dt = time_end - time; }
  return dt;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
