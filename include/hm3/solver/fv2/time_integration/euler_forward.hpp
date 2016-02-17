#pragma once
/// \file
///
///
#include <hm3/types.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {

struct euler_forward {
  /// \name Block state
  ///@{
  struct euler_forward_state {};
  template <typename BlockBase> static euler_forward_state state_t(BlockBase&&);
  ///@}  // Block State

  /// \name Time integration state
  ///@{
  bool done_ = false;
  ///@}  // Time integration state

  template <typename State> constexpr void initialize(State&&) noexcept {
    done_ = false;
  }
  /// Done?
  bool done() const noexcept { return done_; }

  template <typename Block> auto rhs(Block&& b) const noexcept {
    return [&](auto&& c) -> auto { return b.rhs(c); };
  }

  template <typename Block> auto lhs(Block&& b) const noexcept {
    return [&](auto&& c) -> auto { return b.variables(c); };
  }

  template <typename Block> auto rhs(Block&& b) noexcept {
    return [&](auto&& c) -> auto { return b.rhs(c); };
  }

  template <typename Block> auto lhs(Block&& b) noexcept {
    return [&](auto&& c) -> auto { return b.variables(c); };
  }

  /// Advance Block
  template <typename State> void advance(State&& s, num_t dt) noexcept {
    for (auto&& b : s.blocks()) {
      b.for_each_internal([&](auto&& c) { b.variables(c) += dt * b.rhs(c); });
    }
    done_ = true;
  }
};

}  // namespace fv
}  // namespace solver
}  // namespace hm3
