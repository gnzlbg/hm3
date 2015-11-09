#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/equation_of_state.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/state.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace euler {

/// Local Lax-Friedrichs Flux
///
/// Computes the \p d-th component of the Local-Lax_Friedrichs flux at
/// the surface between the cells \p lIdx and \p rIdx. The distance between
/// the cell centers is \p dx and the time-step is \p dt
template <typename V,  typename VT, typename State>
static constexpr auto local_lax_friedrichs(VT&& vt, V&& v_l, V&& v_r, suint_t d,
                                           State&& s) noexcept {
  auto fL = vt.flux(v_l, d);
  auto fR = vt.flux(v_r, d);
  return (0.5 * (fL + fR + s.dx / s.dt * (v_l - v_r)));
}

}  // namespace euler

}  // namespace fv
}  // namespace solver
}  // namespace hm3
