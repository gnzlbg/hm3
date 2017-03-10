#pragma once
/// \file
///
///
#include <hm3/math/core.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3::solver::fv::heat {

struct time_step_fn {
  static constexpr bool requires_cell_state = false;
  ///
  template <typename S>
  constexpr num_t operator()(S const& s, num_t length, num_t cfl) const
   noexcept {
    return cfl * 0.5 * std::pow(length, 2.) / s.thermal_diffusivity;
  }
};

/// Default time step method
namespace {
constexpr auto const& time_step = static_const<time_step_fn>::value;
}  // namespace

}  // namespace hm3::solver::fv::heat
