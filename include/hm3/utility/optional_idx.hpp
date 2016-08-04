#pragma once
/// \file
///
/// Optional index
#include <hm3/utility/compact_optional.hpp>
#include <hm3/utility/math.hpp>

namespace hm3 {

template <typename T, typename Tag, T value = math::highest<T>>
using optional_idx = compact_optional<empty_scalar_value<T, value>, Tag>;

}  // namespace hm3
