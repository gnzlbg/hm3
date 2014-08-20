#pragma once
/// \file
///
/// Performs a cold computation (prevents inlining)

namespace hm3 {

/// Performs a cold computation preventing inlining and branch prediction
template <typename F>
[[ noinline, cold ]] constexpr decltype(auto) cold_do(F&& f) noexcept(
 noexcept(f())) {
  return f();
}

}  // namespace hm3
