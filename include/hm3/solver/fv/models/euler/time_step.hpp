#pragma once
/// \file
///
///
#include <hm3/solver/fv/models/euler/state.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::solver::fv::euler {

struct max_wave_speed_time_step_fn {
  template <typename V, typename S>
  constexpr num_t operator()(V&& v, S const& s, num_t length, num_t cfl) const
   noexcept {
    num_t a = s.speed_of_sound(v);

    /// Compute maximum wave speed: s_max = max(|u_d| + a)
    num_t s_max = (s.u(v).array().abs() + a).maxCoeff();

    return cfl * length / s_max;
  }
};

namespace {
constexpr auto const& max_wave_speed_time_step
 = static_const<max_wave_speed_time_step_fn>::value;
}  // namespace

/// Default time step method
namespace {
constexpr auto const& time_step
 = static_const<max_wave_speed_time_step_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv::euler
