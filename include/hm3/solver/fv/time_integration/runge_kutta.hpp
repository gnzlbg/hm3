#pragma once
/// \file
///
/// Runge Kutta SSP (Strong Stability Preserving) time integration
///
/// It requires an extra copy of the variables per cell.
#include <array>
#include <hm3/types.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace solver {
namespace fv {

template <uint_t NoStages>  //
struct runge_kutta {
  /// \name Block state
  ///@{
  template <typename BlockBase> struct runge_kutta_block_state {
    using var_t   = typename BlockBase::var_t;
    using index_t = typename BlockBase::index_t;
    var_t new_variables_;
    var_t& new_variables() { return new_variables_; }
    var_t const& new_variables() const { return new_variables_; }
    auto new_variables(sidx_t c) noexcept { return new_variables_.row(c); }
    auto new_variables(sidx_t c) const noexcept {
      return new_variables_.row(c);
    }
    auto new_variables(index_t c) noexcept { return new_variables(c.idx); }
    auto new_variables(index_t c) const noexcept {
      return new_variables(c.idx);
    }
  };

  template <typename BlockBase, typename BlockBase_ = std::decay_t<BlockBase>>
  static runge_kutta_block_state<BlockBase_> state_t(BlockBase&&);
  ///@}  // Block State

  /// \name Time integration state
  ///@{
  using coefficients_t = std::array<num_t, NoStages>;
  coefficients_t coefficients;
  uint_t stage = 0;
  ///@}  // Time integration state

  runge_kutta()                   = default;
  runge_kutta(runge_kutta const&) = default;
  runge_kutta& operator=(runge_kutta const&) = default;
  runge_kutta(runge_kutta&&)                 = default;
  runge_kutta& operator=(runge_kutta&&) = default;

  constexpr runge_kutta(coefficients_t coefficients_)
   : coefficients(std::move(coefficients_)) {}

  template <typename State> void initialize(State&& s) {
    stage = 0;
    for (auto&& b : s.blocks()) { b.new_variables() = b.variables(); }
  }

  bool done() const noexcept { return stage == coefficients.size(); }

  template <typename Block> auto rhs(Block&& b) const noexcept {
    return [&](auto&& c) -> decltype(auto) { return b.rhs(c); };
  }

  template <typename Block> auto lhs(Block&& b) const noexcept {
    return [&](auto&& c) -> decltype(auto) { return b.new_variables(c); };
  }

  template <typename Block> auto rhs(Block&& b) noexcept {
    return [&](auto&& c) -> decltype(auto) { return b.rhs(c); };
  }

  template <typename Block> auto lhs(Block&& b) noexcept {
    return [&](auto&& c) -> decltype(auto) { return b.new_variables(c); };
  }

  template <typename State> void advance(State& s, num_t dt) noexcept {
    const num_t f = coefficients[stage] * dt;
    for (auto&& b : s.blocks()) {
      b.for_each_internal(
       [&](auto&& c) { b.new_variables(c) = b.variables(c) + f * b.rhs(c); });
    }
    ++stage;
    if (stage == coefficients.size()) {
      for (auto&& b : s.blocks()) { b.variables() = b.new_variables(); }
    }
  }
};

struct runge_kutta_5_t : runge_kutta<5> {
  constexpr runge_kutta_5_t()
   : runge_kutta<5>(
      std::array<num_t, 5>{{1. / 4., 1. / 6., 3. / 8., 1. / 2., 1.}}) {}
};

namespace {
constexpr auto&& runge_kutta_5 = static_const<runge_kutta_5_t>::value;
}  // namespace

}  // namespace fv
}  // namespace solver
}  // namespace hm3
