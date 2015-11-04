#pragma once
/// \file
///
///
#include <hm3/geometry/dimensions.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <uint_t Nd> struct advection : dimensional<Nd> {
  static constexpr uint_t no_variables() noexcept { return 1; }
  static constexpr auto variables() noexcept {
    return view::iota(0_u, no_variables());
  }
  static string names(uint_t) { return "tracer"; }
  num_t velocity;

  advection(num_t velocity_) : velocity{velocity_} {}

  template <typename State, typename CellData>
  void load(State const&, CellData&&) const noexcept {}
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
