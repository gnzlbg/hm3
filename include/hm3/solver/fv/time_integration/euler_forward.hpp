#pragma once
/// \file
///
///
#include <hm3/solver/fv/tile/lhs.hpp>
#include <hm3/solver/fv/tile/rhs.hpp>
#include <hm3/types.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct euler_forward {
  /// \name Time integration state
  ///@{

  /// Cell variables
  struct tile_variables {
    template <typename Grid, typename Physics,
              typename Order = dense::col_major_t>
    using invoke             = meta::list<            //
     right_hand_side<Grid, Physics::nvars(), Order>,  //
     left_hand_side<Grid, Physics::nvars(), Order>    //
     >;
  };

  bool done_ = false;
  ///@}  // Time integration state

  /// Initializes the time integration
  template <typename State> constexpr void initialize(State&& s) noexcept {
    done_ = false;
  }

  /// Initializes the time integration step
  template <typename State> constexpr void initialize_step(State&& s) noexcept {
    for (auto&& t : s.tiles()) { t.rhs()().fill(0.); }
  }

  /// Time integration done?
  bool done() const noexcept { return done_; }

  /// \name LHS/RHS of the current step
  ///@{

  template <typename Tile> auto rhs(Tile&& t) const noexcept {
    return [&](auto&& c) -> auto { return t.rhs(c); };
  }

  template <typename Tile> auto lhs(Tile&& t) const noexcept {
    return [&](auto&& c) -> auto { return t.lhs(c); };
  }

  template <typename Tile> auto rhs(Tile&& t) noexcept {
    return [&](auto&& c) -> auto { return t.rhs(c); };
  }

  template <typename Tile> auto lhs(Tile&& t) noexcept {
    return [&](auto&& c) -> auto { return t.lhs(c); };
  }

  ///@} // LHS/RHS of the current step

  /// Advance solution by one Euler-Forward step
  template <typename State> void advance(State&& s, num_t dt) noexcept {
    for (auto&& t : s.tiles()) {
      t.cells().for_each_internal([&](auto&& c) { t.lhs(c) += dt * t.rhs(c); });
    }
    done_ = true;
  }
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
