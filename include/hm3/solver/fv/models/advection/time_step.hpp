#pragma once
/// \file
///
///
#include <hm3/solver/fv/models/advection/state.hpp>

namespace hm3::solver::fv::advection {

struct max_wave_speed_time_step_fn {
  template <typename V, typename S, typename B, typename C>
  constexpr num_t operator()(V&& v, S const& s, num_t length, num_t cfl) const
   noexcept {
    num_t u_max = math::lowest<num_t>;
    for (auto d : s.dimensions()) {
      u_max = std::max(u_max, s.max_wave_speed(v, d));
    }

    return cfl * length / u_max;
  }
};

namespace {
constexpr auto&& max_wave_speed_time_step
 = static_const<max_wave_speed_time_step_fn>::value;
}  // namespace

/// Default time step method
namespace {
constexpr auto&& time_step = static_const<max_wave_speed_time_step_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv::advection
