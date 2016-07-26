#pragma once
/// \file
///
/// Compute time step
#include <hm3/types.hpp>
#include <hm3/utility/math.hpp>
#include <hm3/utility/range.hpp>
#include <limits>

namespace hm3 {
namespace solver {
namespace fv {

struct compute_time_step_fn {
  template <typename State, typename TimeStepF, typename V>
  constexpr num_t impl(State const& s, num_t cfl, TimeStepF&& tf, V&& v,
                       std::true_type) const noexcept {
    num_t dt = math::highest<num_t>;
    for (auto&& t : s.tiles()) {
      auto&& lhs = s.time_integration.lhs(t);
      auto dx    = t.geometry().cell_length();
      t.cells().for_each_internal(
       [&](auto c) { dt = std::min(dt, tf(lhs(c), v, dx, cfl)); });
    }
    return dt;
  }

  template <typename State, typename TimeStepF, typename V>
  constexpr num_t impl(State const& s, num_t cfl, TimeStepF&& tf, V&& v,
                       std::false_type) const noexcept {
    num_t dx = math::highest<num_t>;
    for (auto&& t : s.tiles()) {
      dx = std::min(dx, t.geometry().cell_length());
    }
    return tf(v, dx, cfl);
  }

  template <typename State, typename TimeStepF, typename V>
  constexpr num_t operator()(State const& s, num_t cfl, TimeStepF&& t, V&& v,
                             num_t time, num_t time_end) const noexcept {
    constexpr bool requires_cell_state
     = uncvref_t<TimeStepF>::requires_cell_state;
    num_t dt = impl(s, cfl, t, v, meta::bool_<requires_cell_state>{});
    if (time + dt > time_end) { dt = time_end - time; }
    return dt;
  }
};

namespace {
static constexpr auto&& compute_time_step
 = static_const<compute_time_step_fn>::value;
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
