#pragma once
/// \file
///
///
#include <hm3/solver/fv/advection/state.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

struct max_wave_speed_time_step_fn {
  template <typename V, typename S, typename B, typename C>
  constexpr num_t operator()(V&& v, S const& s, num_t length, num_t cfl) const
   noexcept {
    num_t u_max = std::numeric_limits<num_t>::lowest();
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

}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
