#pragma once
/// \file
///
/// Dimension of an object
#include <hm3/utility/range.hpp>

namespace hm3 {
namespace geometry {

template <typename T>
constexpr auto dimension(T&& t) noexcept -> decltype(t.dimension()) {
  return t.dimension();
}

template <typename T>
using dimension_t = ranges::uncvref_t<decltype(dimension(std::declval<T>()))>;

}  // namespace geometry
}  // namespace hm3
