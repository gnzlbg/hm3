#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/equation_of_state.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/state.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

namespace flux {

struct local_lax_friedrichs_fn {
  /// Local Lax-Friedrichs Flux
  ///
  /// Computes the \p d-th component of the Local-Lax_Friedrichs flux at an
  /// interface with left \p v_l and right \p v_r states.
  template <typename V, typename VT, typename State>
  constexpr auto operator()(VT&& vt, V&& v_l, V&& v_r, suint_t d,
                            State&& s) const noexcept {
    return (0.5
            * (vt.flux(v_l, d) + vt.flux(v_r, d) + s.dx / s.dt * (v_l - v_r)))
     .eval();
  }
};

namespace {
constexpr auto&& local_lax_friedrichs
 = static_const<local_lax_friedrichs_fn>::value;
}  // namespace

}  // namespace flux
}  // namespace euler
}  // namespace fv
}  // namespace solver
}  // namespace hm3
