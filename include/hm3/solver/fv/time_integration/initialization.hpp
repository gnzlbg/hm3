#pragma once
/// \file
///
/// Initializes the time integration
namespace hm3 {
namespace solver {
namespace fv {

template <typename State> void initialize_time_integration(State& s) {
  s.time_integration.initialize(s);
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
