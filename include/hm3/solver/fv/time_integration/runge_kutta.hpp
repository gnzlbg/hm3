#pragma once
/// \file
///
/// Runge Kutta SSP (Strong Stability Preserving) time integration
///
/// It requires an extra copy of the variables per cell.
#include <hm3/solver/fv/tile/lhs.hpp>
#include <hm3/solver/fv/tile/rhs.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <suint_t NoStages>  //
struct runge_kutta {
  /// \name Time integration state
  ///@{

  /// Cell variables
  struct tile_variables {
    template <typename Grid, typename Physics,
              typename Order = dense::col_major_t>
    using invoke             = meta::list<              //
     right_hand_side<Grid, Physics::nvars(), Order>,    //
     left_hand_side<Grid, Physics::nvars(), Order>,     //
     new_left_hand_side<Grid, Physics::nvars(), Order>  //
     >;
  };

  using coefficients_t = array<num_t, NoStages>;
  coefficients_t coefficients;
  uint_t stage = 0;
  ///@}  // Time integration state

  /// Initialize the time integration
  ///
  /// Sets Runge-Kutta stage to zero and copies the LHS into the Runge-Kutta
  /// intermediate storage (new_lhs)
  template <typename State> void initialize(State&& s) {
    stage = 0;
    for (auto&& t : s.tiles()) { t.new_lhs() = t.lhs(); }
  }

  /// Initializes the time integration step
  template <typename State> constexpr void initialize_step(State&& s) noexcept {
    for (auto&& t : s.tiles()) { t.rhs()().fill(0.); }
  }

  /// Time integration done?
  bool done() const noexcept { return stage == coefficients.size(); }

  constexpr runge_kutta(coefficients_t coefficients_)
   : coefficients(std::move(coefficients_)) {}

  /// \name LHS/RHS of the current step
  ///@{

  template <typename Tile> auto rhs(Tile&& t) const noexcept {
    return [&](auto&& c) -> decltype(auto) { return t.rhs(c); };
  }

  template <typename Tile> auto rhs(Tile&& t) noexcept {
    return [&](auto&& c) -> decltype(auto) { return t.rhs(c); };
  }

  template <typename Tile> auto lhs(Tile&& t) const noexcept {
    return [&](auto&& c) -> decltype(auto) { return t.new_lhs(c); };
  }

  template <typename Tile> auto lhs(Tile&& t) noexcept {
    return [&](auto&& c) -> decltype(auto) { return t.new_lhs(c); };
  }

  ///@} // LHS/RHS of the current step

  /// Advance solution by one Runge-Kutta step
  template <typename State>
  [[ HM3_HOT, HM3_FLATTEN ]] void advance(State& s, num_t dt) noexcept {
    if (stage == coefficients.size()) {
      HM3_FATAL_ERROR("time integration finished: call `initialize()`");
    }
    const num_t f = coefficients[stage] * dt;
    for (auto&& t : s.tiles()) {
      t.cells().for_each_internal(
       [&](auto&& c) { t.new_lhs(c) = t.lhs(c) + f * t.rhs(c); });
    }
    ++stage;
    if (stage == coefficients.size()) {
      // TODO: Optimization: this should just swap pointers (not perform a copy)
      for (auto&& t : s.tiles()) { t.lhs() = t.new_lhs(); }
    }
  }
};

struct runge_kutta_5 : runge_kutta<5> {
  constexpr runge_kutta_5()
   : runge_kutta<5>(array<num_t, 5>{{1. / 4., 1. / 6., 3. / 8., 1. / 2., 1.}}) {
  }
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
