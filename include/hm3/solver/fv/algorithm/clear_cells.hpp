#pragma once
/// \file
///
/// Clear algorithms

namespace hm3 {
namespace solver {
namespace fv {

/// Clears the Right Hand Side of all tiles
template <typename State> void clear_rhs(State& s) {
  for (auto&& b : s.tiles()) { b.rhs()().fill(0.); }
  // for (auto&& b : s.tiles()) { b.fluxes_().fill(0.); }
  // for (auto&& b : s.tiles()) { b.gradients_().fill(0.); }
}

/// Clears the Halo cells of all tiles
///
/// TODO: enable only in debug mode (their values should always be overwritten)
template <typename State> void clear_halos(State& s) {
  for (auto&& b : s.tiles()) {
    b.cells().for_each_halo([&](auto&& c) {
      b.variables(c.idx).fill(0.);
      // flux
      // gradient
      //
    });
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
