#pragma once

namespace hm3 {
namespace solver {
namespace fv {

template <typename State> void cv_to_pv(State& s) {
  for (auto&& b : s.tiles()) {
    b.cells().for_each([&](auto&& c) { s.physics.cv_to_pv(b.variables(c)); });
  }
}

template <typename State> void pv_to_cv(State& s) {
  for (auto&& b : s.tiles()) {
    b.cells().for_each([&](auto&& c) { s.physics.pv_to_cv(b.variables(c)); });
  }
}

}  // namespace fv
}  // namespace solver
}  // namespace hm3
