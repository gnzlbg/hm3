#pragma once
/// \file
///
/// Performs a cold computation (prevents inlining)
#include <hm3/config/attributes.hpp>

namespace hm3 {

/// Performs a cold computation preventing inlining and branch prediction
template <typename F>
[[ HM3_NO_INLINE, HM3_COLD ]] constexpr decltype(auto) cold_do(F&& f) noexcept(
 noexcept(f())) {
  return f();
}

}  // namespace hm3
