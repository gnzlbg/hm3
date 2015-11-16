#pragma once
/// \file
///
///

namespace hm3 {
namespace solver {
namespace fv {

template <typename State> void initialize_time_integration(State& s) {
  for (auto&& b : s.blocks()) { b.time_integration.initialize(b); }
}

struct euler_forward {
  /// \name Block state
  ///@{
  struct euler_forward_state {};
  template <typename BlockBase> static euler_forward_state state_t(BlockBase&&);
  ///@}  // Block State

  /// \name Time integration state
  ///@{
  bool done = false;
  ///@}  // Time integration state

  template <typename State> constexpr void initialize(State&&) noexcept {
    done = false;
  }
  /// Done?
  bool done() const noexcept { return done_; }

  /// Advance Block
  template <typename State>
  static constexpr void advance_rhs(State& s, num_t dt) noexcept {
    for (auto&& b : s.blocks()) { b.variables() += dt * b.rhs(); }
  }

  /// Advance time integration
  template <typename State> void advance(State&&) noexcept { done = true; }
};

struct runge_kutta {
  /// \name Block state
  ///@{
  template <typename BlockBase> struct runge_kutta_state {
    using var_t = typename BlockBase::var_t;
    var_t new_variables_;
    var_t& new_variables() { return new_variables_; }
    var_t const& new_variables() const { return new_variables_; }
  };

  template <typename BlockBase, typename BlockBase_ = std::decay_t<BlockBase>>
  static runge_kutta_state<BlockBase_> state_t(BlockBase&&);
  ///@}  // Block State

  /// \name Time integration state
  ///@{
  std::array<num_t, 5> coefficients{{1. / 4., 1. / 6., 3. / 8., 1. / 2., 1.}};
  uint_t stage = 0;
  ///@}  // Time integration state

  template <typename State> void initialize(State&& s) {
    stage = 0;
    for (auto&& b : s.blocks()) { b.new_variables() = b.variables(); }
  }

  explicit operator bool() { return stage == coefficients.size(); }

  template <typename State> void advance_rhs(State& s, num_t dt) {
    const num_t f = coefficients[stage] * dt;
    for (auto&& b : s.blocks()) {
      b.new_variables() = b.variables() - f * b.rhs();
    }
  }

  template <typename State> void advance(State& s) noexcept {
    ++stage;
    if (stage == coefficients.size()) {
      for (auto&& b : s.blocks()) { b.variables() = b.new_variables(); }
    }
  }
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
