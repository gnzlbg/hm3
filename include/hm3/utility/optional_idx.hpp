#pragma once
/// \file
///
/// Optional index
#include <hm3/math/core.hpp>
#include <hm3/utility/compact_optional.hpp>

namespace hm3 {

template <typename T, typename Tag, T Value = math::highest<T>>
using optional_idx = compact_optional<empty_scalar_value<T, Value>, Tag>;

}  // namespace hm3
