#pragma once
/// \file
///
#include <hm3/solver/fv/algorithm/structured_flux.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <typename State, typename NumFluxF, typename VT>
void compute_structured_rhs(State&& s, NumFluxF&& nf, num_t dt, VT&& vt) {
  for (auto&& b : s.tiles()) {
    auto&& rhs = s.time_integration.rhs(b);
    b.cells().for_each_internal([&](auto c) {
      rhs(c) += structured_numerical_flux(nf, dt, vt, b, c,
                                          s.time_integration.lhs(b));
    });
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
