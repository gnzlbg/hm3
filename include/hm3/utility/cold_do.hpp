#pragma once
/// \file cold_do.hpp Performs a cold computation (prevents inlining)
#include <hm3/utility/returns.hpp>

namespace hm3 {

/// Performs a cold computation preventing inlining and branch prediction
template <class F>
[[ noinline, cold ]] constexpr auto cold_do(F&& f) HM3_RETURNS(f());

}  // namespace hm3
