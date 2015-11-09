#pragma once
/// \file
///
///
#include <hm3/solver/fv/euler/state.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

template <typename V, typename S>
static constexpr num_t time_step(V&& v, S const& s, num_t length,
                                 num_t cfl) noexcept {
  num_t a = s.speed_of_sound(v);

  /// Compute maximum wave speed: s_max = max(|u_d| + a)
  num_t s_max = (s.u(v).array().abs() + a).maxCoeff();

  return cfl * length / s_max;
}

}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
