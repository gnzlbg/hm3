#pragma once
/// \file
///
/// Rusanov's numerical flux
#include <hm3/geometry/dimensions.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {

namespace flux {

struct rusanov_fn {
  /// Rusanov Flux
  ///
  /// Computes the \p d-th component of Rusanov's flux at an interface with left
  /// \p v_l and right \p v_r states.
  template <typename V, typename VT, typename State,
            typename var_v = num_a<std::decay_t<VT>::nvars()>>
  constexpr auto operator()(VT&& vt, V&& v_l, V&& v_r, dim_t d,
                            State&& /*s*/) const noexcept {
    num_t max_wave_speed
     = std::max(vt.max_wave_speed(v_l, d), vt.max_wave_speed(v_r, d));
    var_v f
     = 0.5 * (vt.flux(v_l, d) + vt.flux(v_r, d) + max_wave_speed * (v_l - v_r));
    return f;
  }
};

namespace {
constexpr auto&& rusanov = static_const<rusanov_fn>::value;
}  // namespace

}  // namespace flux

}  // namespace fv
}  // namespace solver
}  // namespace hm3
