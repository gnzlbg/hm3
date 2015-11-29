#pragma once
/// \file
///
///
#include <hm3/solver/fv/time_integration/euler_forward.hpp>
#include <hm3/solver/fv/time_integration/runge_kutta.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename State> void initialize_time_integration(State& s) {
  s.time_integration.initialize(s);
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
